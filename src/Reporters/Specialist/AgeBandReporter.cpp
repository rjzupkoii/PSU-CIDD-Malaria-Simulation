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
  // Create the configuration for the PfPR reporter
  el::Configurations pfpr_reporter;
  pfpr_reporter.setToDefault();
  pfpr_reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  pfpr_reporter.setGlobally(el::ConfigurationType::ToFile, "true");
  pfpr_reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}age_band_pfpr_{}.csv", path, job_number));
  pfpr_reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  pfpr_reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("age_band_pfpr", pfpr_reporter);

  // Create the configuration for the cases and population reporter
  el::Configurations cases_reporter;
  cases_reporter.setToDefault();
  cases_reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  cases_reporter.setGlobally(el::ConfigurationType::ToFile, "true");
  cases_reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}age_band_cases_{}.csv", path, job_number));
  cases_reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  cases_reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("age_band_cases", cases_reporter);

  // Determine when we should start running, we want the last year of data, accounting for leap years.
  start_recording = Model::CONFIG->total_time();
  start_recording -= 366;
  LOG(INFO) << "Logging of age-banded blood slide prevalence will start at model day " << start_recording;

  // Build a lookup for location to district
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    district_lookup.emplace_back(SpatialData::get_instance().get_district(loc));
  }

  // Log the header for the PfPR and cases file, this is a bit redundant because we are writing to two different immutable streams
  pfpr << "ModelDays" << Csv::sep << "District" << Csv::sep;
  cases << "ModelDays" << Csv::sep << "District" << Csv::sep;
  for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
    pfpr << Model::CONFIG->age_structure()[ac] << Csv::sep;
    auto band = Model::CONFIG->age_structure()[ac];
    cases << "cases_" << band << Csv::sep << "pop_" << band << Csv::sep;
  }
  pfpr << Csv::end_line;
  cases << Csv::end_line;
  CLOG(INFO, "age_band_pfpr") << pfpr.str();
  CLOG(INFO, "age_band_cases") << cases.str();
  pfpr.str("");
  cases.str("");
}

void AgeBandReporter::monthly_report() {
  // Should we start reporting?
  auto current_time = Model::SCHEDULER->current_time();
  if (current_time < start_recording) { return; }

  // Prepare some values
  auto age_classes = Model::CONFIG->number_of_age_classes();
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  std::vector<std::vector<int>> population(districts, std::vector<int>(age_classes));
  std::vector<std::vector<double>> prevalence(districts, std::vector<double>(age_classes));

  // Iterate over all possible locations
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      // Note the district we are in, make sure things are zero indexed
      auto district = district_lookup[loc] - first_index;

      // Update the population and case data
      population[district][ac] += Model::DATA_COLLECTOR->popsize_by_location_age_class()[loc][ac];
      prevalence[district][ac] += Model::DATA_COLLECTOR->blood_slide_number_by_location_age_group()[loc][ac];
    }
  }

  // We now have the data, write the pfpr out to disk
  for (auto district = 0; district < districts; district++) {
    pfpr << current_time << Csv::sep << (district + first_index) << Csv::sep;
    cases << current_time << Csv::sep << (district + first_index) << Csv::sep;
    for (auto ac = 0; ac < age_classes; ac++) {
      pfpr << ((prevalence[district][ac] != 0) ? (prevalence[district][ac] / population[district][ac]) * 100.0 : 0) << Csv::sep;
      cases << prevalence[district][ac] << Csv::sep << population[district][ac] << Csv::sep;
    }
    pfpr << Csv::end_line;
    cases << Csv::end_line;
  }
  CLOG(INFO, "age_band_pfpr") << pfpr.str();
  CLOG(INFO, "age_band_cases") << cases.str();
  pfpr.str("");
  cases.str("");
}
