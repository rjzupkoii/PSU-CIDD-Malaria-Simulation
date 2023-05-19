/*
 * AgeBandReporter.cpp
 *
 * Implementation of the AgeBandReporter class.
 */
#include "AgeBandReporter.h"

#include "easylogging++.h"

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"

void AgeBandReporter::initialize(int job_number, std::string path) {
  // Create the configuration for the reporter
  el::Configurations reporter;
  reporter.setToDefault();
  reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  reporter.setGlobally(el::ConfigurationType::ToFile, "true");
  reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}age_band_data_{}.csv", path, job_number));
  reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("age_band_reporter", reporter);

  // Determine when we should start running, we want the last year of data, accounting for leap years.
  start_recording = Model::CONFIG->total_time();
  start_recording -= 366;
  LOG(INFO) << "Logging of age-banded blood slide prevalence will start at model day " << start_recording;

  // Build a lookup for location to district
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    district_lookup.emplace_back(SpatialData::get_instance().get_district(loc));
  }

  // Log the header for the file
  ss << "District" << Csv::sep;
  for (auto age = 1; age <= ZERO_TO_TEN; age++) {
    ss << age - 1 << "-" << age << Csv::sep;
  }
  for (auto age = 1; age <= FIFTEEN_TO_HUNDRED; age++) {
    ss << (15 + (age - 1) * 5) << "-" << (15 + age * 5) << Csv::sep;
  }
  ss << Csv::end_line;
  CLOG(INFO, "age_band_reporter") << ss.str();
  ss.str("");
}

void AgeBandReporter::monthly_report() {
  // Should we start reporting?
  if (Model::SCHEDULER->current_time() < start_recording) {
    return;
  }

  // Prepare some values
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  std::vector<int> population(districts);
  std::vector<std::vector<double>> prevalence(districts, std::vector<double>(ZERO_TO_TEN + FIFTEEN_TO_HUNDRED));

  // Iterate over all possible locations
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    // If the population is zero, press on
    auto location_population = Model::DATA_COLLECTOR->popsize_by_location()[location];
    if (location_population == 0) { continue; }

    // Note the district we are in, make sure things are zero indexed
    auto district = district_lookup[location] - first_index;

    // Update the total population for the district
    population[district] += location_population;

    // Iterate over the ten-and-under age bands
    auto index = 0;
    for (auto age = 1; age <= ZERO_TO_TEN; age++) {
      prevalence[district][index] += (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, age - 1, age) * location_population);
      index++;
    }
    for (auto age = 1; age <= FIFTEEN_TO_HUNDRED; age++) {
      prevalence[district][index] += (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 15 + ((age - 1) * 5), 15 + (age * 5)) * location_population);
      index++;
    }
  }

  // We now have the data, write it out to disk
  for (auto district = 0; district < districts; district++) {
    ss << (district + first_index) << Csv::sep;
    for (auto ndx = 0; ndx < (ZERO_TO_TEN + FIFTEEN_TO_HUNDRED); ndx++) {
      auto result = (prevalence[district][ndx] != 0) ? (prevalence[district][ndx] / population[district]) * 100.0 : 0;
      ss << result << Csv::sep;
    }
    ss << Csv::end_line;
  }
  CLOG(INFO, "age_band_reporter") << ss.str();
  ss.str("");
}
