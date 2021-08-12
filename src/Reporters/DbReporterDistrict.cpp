/*
 * DbReporterDistrict.cpp
 *
 * Implement the DbReporterDistrict class.
 */
#include "DbReporterDistrict.h"

#include "easylogging++.h"

#include "Core/Config/Config.h"
#include "Model.h"
#include "MDC/ModelDataCollector.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

void DbReporterDistrict::initialize(int job_number, std::string path) {
  // Inform the user of the reporter type and make sure there are districts
  VLOG(1) << "Using DbReporter with aggregation at the district level.";
  if (!SpatialData::get_instance().has_raster(SpatialData::Districts)) {
    LOG(ERROR) << "District raster must be present when aggregating data at the district level.";
    throw std::invalid_argument("No district raster present");
  }

  // Defer to the base class for the remainder
  DbReporter::initialize(job_number, path);
}

void DbReporterDistrict::monthly_genome_data(int id, std::string &query) {

  // Cache some values
  auto genotypes = Model::CONFIG->number_of_parasite_types();
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = index->vPerson()[0][0].size();

  // Prepare the data structures
  std::vector<int> individual(genotypes, 0);
  std::vector<std::vector<int>> occurrences(districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> clinicalOccurrences(districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> occurrencesZeroToFive(districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> occurrencesTwoToTen(districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> weightedOccurrences(districts, std::vector<int>(genotypes, 0));

  // Iterate over all the possible states
  for (unsigned int location = 0; location < index->vPerson().size(); location++) {

    // Get the current index and apply the off set so we are zero aligned
    auto district = SpatialData::get_instance().get_district(location) - first_index;
    int infectedIndividuals = 0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      // Iterate over all the age classes
      for (unsigned int ac = 0; ac < age_classes; ac++) {
        // Iterate over all the genotypes
        auto age_class = index->vPerson()[location][hs][ac];
        for (auto &person : age_class) {

          // Get the person, press on if they are not infected
          auto parasites = person->all_clonal_parasite_populations()->parasites();
          auto size = parasites->size();
          if (size == 0) { continue; }

          // Note the age and clinical status of the person
          auto age = person->age();
          auto clinical = (int)(person->host_state() == Person::HostStates::CLINICAL);

          // Update the count of infected individuals
          infectedIndividuals++;

          // Count the genotypes present in the individual
          for (unsigned int ndx = 0; ndx < size; ndx++) {
            auto parasite_population = (*parasites)[ndx];
            auto id = parasite_population->genotype()->genotype_id();
            occurrences[district][id]++;
            occurrencesZeroToFive[district][id] += (age <= 5);
            occurrencesTwoToTen[district][id] += (age >= 2 && age <= 10);
            individual[id]++;

            // Count a clinical occurrence if the individual has clinical symptoms
            clinicalOccurrences[district][id] += clinical;
          }

          // Update the weighted occurrences and reset the individual count
          auto divisor = static_cast<double>(size);
          for (unsigned int ndx = 0; ndx < genotypes; ndx++) {
            weightedOccurrences[district][ndx] += (individual[ndx] / divisor);
            individual[ndx] = 0;
          }
        }
      }
    }

    // Update the number of individuals in the location
    query.append(fmt::format(UPDATE_INFECTED_INDIVIDUALS, infectedIndividuals, id, location_index[location]));
  }

  // Iterate over the districts and append the query
  query.append(INSERT_GENOTYPE_PREFIX);
  for (auto district = 0; district < districts; district++) {
    for (auto genotype = 0; genotype < genotypes; genotype++) {
      if (weightedOccurrences[district][genotype] == 0) { continue; }
      query.append(fmt::format(INSERT_GENOTYPE_ROW,
        id, (district + first_index), genotype,
        occurrences[district][genotype],
        clinicalOccurrences[district][genotype],
        occurrencesZeroToFive[district][genotype],
        occurrencesTwoToTen[district][genotype],
        weightedOccurrences[district][genotype]));
    }
  }
  query[query.length() - 1] = ';';
}

void DbReporterDistrict::monthly_site_data(int id, std::string &query) {

  // Prepare the data structures
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  std::vector<double> eir(districts, 0);
  std::vector<double> pfpr_under5(districts, 0);
  std::vector<double> pfpr_2to10(districts, 0);
  std::vector<double> pfpr_all(districts, 0);
  std::vector<int> population(districts, 0);
  std::vector<int> clinical_episodes(districts, 0);
  std::vector<int> treatments(districts, 0);
  std::vector<int> treatment_failures(districts, 0);
  std::vector<int> nontreatment(districts, 0);

  // Collect the data
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {

    // If the population is zero, press on
    auto location_population = Model::DATA_COLLECTOR->popsize_by_location()[location];
    if (location_population == 0) { continue; }

    // Note the district we are in, make sure things are zero indexed
    auto district = SpatialData::get_instance().get_district(location) - first_index;

    // Collect the simple data
    population[district] += location_population;
    clinical_episodes[district] += Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location];
    treatments[district] += Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location];
    treatment_failures[district] += Model::DATA_COLLECTOR->monthly_treatment_failure_by_location()[location];
    nontreatment[district] += Model::DATA_COLLECTOR->monthly_nontreatment_by_location()[location];

    // EIR and PfPR is a bit more complicated since it could be an invalid value early in the simulation, and when
    // aggregating at the district level the weighted mean needs to be reported instead
    if (Model::DATA_COLLECTOR->recording_data()) {
      auto eir_location = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()
              ? 0 : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
      eir[district] += (eir_location * location_population);
      pfpr_under5[district] = (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * location_population);
      pfpr_2to10[district] = (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * location_population);
      pfpr_all[district] = (Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * location_population);
    }
  }

  // Append the final query
  for (auto district = 0; district < districts; district++) {
    query.append(fmt::format(INSERT_SITE,
      id, (district + first_index),
      population[district],
      clinical_episodes[district],
      treatments[district],
      (eir[district] != 0) ? (eir[district] / population[district]) : 0,
      (pfpr_under5[district] != 0) ? (pfpr_under5[district] / population[district]) * 100.0 : 0,
      (pfpr_2to10[district] != 0) ? (pfpr_2to10[district] / population[district]) * 100.0 : 0,
      (pfpr_all[district] != 0) ? (pfpr_all[district] / population[district]) * 100.0 : 0,
      treatment_failures[district],
      nontreatment[district]));
  }
}