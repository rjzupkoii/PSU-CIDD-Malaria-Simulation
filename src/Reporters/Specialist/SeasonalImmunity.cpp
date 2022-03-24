/*
 * SeasonalImmunity.cpp
 *
 * Implements the SeasonalImmunity class.
 */
#include "SeasonalImmunity.h"

#include "Core/Config/Config.h"
#include "GIS/SpatialData.h"
#include "Helpers/RunningMedian.hxx"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "easylogging++.h"

void SeasonalImmunity::initialize(int job_number, std::string path) {
  // Start by building our look-up
  build_lookup();

  // Prepare the logger to record the data
  auto filename = fmt::format("{}_seasonal_immunity_{}.csv", path, job_number);
  el::Configurations seasonal_logger;
  seasonal_logger.setToDefault();
  seasonal_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  seasonal_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  seasonal_logger.setGlobally(el::ConfigurationType::Filename, filename);
  seasonal_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  seasonal_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "3");
  el::Loggers::reconfigureLogger("seasonal_logger", seasonal_logger);

  // Log the seasonal headers
  ss << "DaysElapsed" << Csv::sep
     << "ClimaticZone" << Csv::sep
     << "Population" << Csv::sep
     << "MedianTheta" << Csv::sep
     << "MeanTheta" << Csv::sep
     << "InfectedIndividuals" << Csv::sep
     << "ClinicalIndividuals" << Csv::sep
     << "NewInfections" << Csv::sep
     << "Treatments" << Csv::sep
     << "NonTreatment" << Csv::sep
     << "TreatmentFailure" << Csv::sep
     << "ParasiteClones" << Csv::sep
     << "Multiclonal" << Csv::sep
     << "580yWeighted" << Csv::sep
     << "580yUnweighted" << Csv::sep
     << "580yMulticlonal" << Csv::sep
     << Csv::end_line;
  CLOG(INFO, "seasonal_logger") << ss.str();
  ss.str("");

  // Note that we are running
  LOG(INFO) << "Seasonal Immunity logging enabled, writing to: " << filename;
}

void SeasonalImmunity::monthly_report() {
  // Hold off on logging until the MDC starts recording, this will help things run a bit faster for large models
  if (!ModelDataCollector::recording_data()) {
    return;
  }

  // Set up our storage, start with individual specific
  std::vector<int> population(lookup_allocation, 0);
  std::vector<RunningMedian<double>> median_theta(lookup_allocation, RunningMedian<double>());
  std::vector<double> mean_theta(lookup_allocation, 0);

  // Location index specific
  std::vector<int> new_infections(lookup_allocation, 0);
  std::vector<int> treatments(lookup_allocation, 0);
  std::vector<int> nontreatment(lookup_allocation, 0);
  std::vector<int> treatment_failure(lookup_allocation, 0);

  // Parasite specific
  std::vector<int> clinical_individuals(lookup_allocation, 0);
  std::vector<int> infected_individuals(lookup_allocation, 0);
  std::vector<int> parasite_clones(lookup_allocation, 0);
  std::vector<int> multiclonal(lookup_allocation, 0);
  std::vector<int> unweighted_580y(lookup_allocation, 0);
  std::vector<int> multiclonal_580y(lookup_allocation, 0);
  std::vector<double> weighted_580y(lookup_allocation, 0);

  // Cache relevant data
  auto* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = index->vPerson()[0][0].size();

  // Iterate over all the locations
  for (auto location = 0; location < index->vPerson().size(); location++) {

    // Update our location specific items
    auto zone = lookup[location];
    new_infections[zone] += Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[location];
    treatments[zone] += Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location];
    nontreatment[zone] += Model::DATA_COLLECTOR->monthly_nontreatment_by_location()[location];
    treatment_failure[zone] += Model::DATA_COLLECTOR->monthly_treatment_failure_by_location()[location];
    clinical_individuals[zone] += Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location];

    // Iterate overall of the individuals in this location
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (unsigned int ac = 0; ac < age_classes; ac++) {
        for (auto &person : index->vPerson()[location][hs][ac]) {

          // Update the population
          population[zone]++;

          // Update the mean and median theta structures
          auto theta = person->immune_system()->get_current_value();
          mean_theta[zone] += theta;
          median_theta[zone].push(theta);

          // Get the parasites, or press on if none are present
          auto parasites = person->all_clonal_parasite_populations()->parasites();
          if (parasites->empty()) { continue; }
          auto size = static_cast<int>(parasites->size());

          // Update the individual infection counts
          infected_individuals[zone]++;

          // Update the multiclonal count if there is more than one parasite clone present
          if (size > 1) {
            multiclonal[zone]++;
          }

          // Update the parasite counts
          parasite_clones[zone] += size;
          for (auto ndx = 0; ndx < size; ndx++) {
            auto parasite = (*parasites)[ndx];

            // Is this is a 580Y parasite?
            if (parasite->genotype()->gene_expression()[2] == 1) {
              unweighted_580y[zone]++;
              weighted_580y[zone] += (1 / static_cast<double>(size));

              // Is this a multiclonal 580Y?
              if (size > 1) {
                multiclonal_580y[zone]++;
              }
            }
          }
        }
      }
    }
  }

  // Log the monthly data for the zones
  for (int zone = 0; zone < lookup_allocation; zone++) {
    ss << Model::SCHEDULER->current_time() << Csv::sep
       << zone << Csv::sep
       << population[zone] << Csv::sep
       << median_theta[zone].getMedian() << Csv::sep
       << mean_theta[zone] / population[zone] << Csv::sep
       << infected_individuals[zone] << Csv::sep
       << clinical_individuals[zone] << Csv::sep
       << new_infections[zone] << Csv::sep
       << treatments[zone] << Csv::sep
       << nontreatment[zone] << Csv::sep
       << treatment_failure[zone] << Csv::sep
       << parasite_clones[zone] << Csv::sep
       << multiclonal[zone] << Csv::sep
       << weighted_580y[zone] << Csv::sep
       << unweighted_580y[zone] << Csv::sep
       << multiclonal_580y[zone] << Csv::sep
       << Csv::end_line;
  }
  CLOG(INFO, "seasonal_logger") << ss.str();
  ss.str("");
}

void SeasonalImmunity::build_lookup() {
  // Get the raster data and make sure it is valid
  AscFile* raster = SpatialData::get_instance().get_raster(SpatialData::SpatialFileType::Ecoclimatic);
  if (raster == nullptr) {
    LOG(ERROR) << "The Seasonal Immunity reporter cannot be run without an associated climate raster.";
    throw std::runtime_error("SeasonalImmunity reporter without underlying raster is not supported.");
  }

  // Load the value into the lookup based on the raster
  for (int row = 0; row < raster->NROWS; row++) {
    for (int col = 0; col < raster->NCOLS; col++) {
      // Pass if we have no data
      if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }

      // Verify the zone
      auto zone = static_cast<int>(raster->data[row][col]);
      if (zone < 0) { throw std::out_of_range(fmt::format("Raster value at row: {}, col: {} is less than zero.", row, col)); }

      // Set the value
      lookup.emplace_back(zone);
      lookup_allocation = (lookup_allocation < zone) ? zone : lookup_allocation;
    }
  }

  // Update the lookup allocation by one to account for indexing
  lookup_allocation++;
}