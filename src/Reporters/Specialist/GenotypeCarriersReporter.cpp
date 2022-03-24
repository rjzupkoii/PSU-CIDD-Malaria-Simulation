/*
 * GenotypeCarriersReporter.cpp
 *
 * Implement the GenotypeCarriersReporter class.
 */
#include "GenotypeCarriersReporter.h"

#include <pqxx/pqxx>

#include "easylogging++.h"

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Reporters/Utility/PqxxHelpers.hxx"

void GenotypeCarriersReporter::initialize(int job_number, std::string path) {
  // Query to verify that the database has been properly configured
  pqxx::connection* connection = pqxx_db::get_connection();
  pqxx::work db(*connection);
  pqxx::result result = db.exec(SELECT_COLUMN);

  // If nothing was returned, throw an error
  if (result.empty()) {
    LOG(ERROR) << "Database not configured correctly for Genotype Carriers Reporter, missing \'sim.monthlysitedata.genotypecarriers\'";
    throw std::runtime_error("Missing Genotype Carriers Reporter column");
  }

  // Since the column is there, we next need to determine the aggregation level
  int replicate = Model::MODEL->replicate();
  if (replicate == 0) {
    LOG(ERROR) << "Database replicate number does not appear to be set, unable to determine aggregation level";
    throw std::runtime_error("Missing database replicate number");
  }
  result = db.exec(fmt::format(SELECT_AGGREGATION, replicate));
  if (result.empty()) {
    LOG(ERROR) << "Database did not return the aggregation level for the study";
    throw std::runtime_error("Aggregation level not returned");
  }
  aggregation = result[0][0].as<std::string>();
  if (aggregation != "C" && aggregation != "D") {
    LOG(ERROR) << "Unknown aggregation level code: " << aggregation;
    throw std::runtime_error("Unknown aggregation code returned");
  }

  // Build the lookup for the mapping
  build_lookup(replicate, connection);

  // Clean up, note that a transaction created, so we need to manage it
  db.abort();
  connection->close();
  delete connection;

  // Inform the user that we are running, note the genotype
  LOG(INFO) << fmt::format("Absolute count of carriers of {} (locus {}, allele {}) is being recorded to database.",
                           GENOTYPE, LOCUS, ALLELE);
  VLOG(1) << "Replicate: " << replicate << ", Aggregation: " << aggregation;
}

void GenotypeCarriersReporter::monthly_report() {
  // Setup our storage
  std::vector<int> genotype_count(lookup_allocation);

  // Cache relevant data
  auto* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = index->vPerson()[0][0].size();

  // Iterate over all the locations and states
  for (auto location = 0; location < index->vPerson().size(); location++) {

    // Reset the count for this location
    int count = 0;

    // Iterate over all the individuals in this location
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (unsigned int ac = 0; ac < age_classes; ac++) {
        auto age_class = index->vPerson()[location][hs][ac];
        for (auto &person : age_class) {
          // Pass if the person is not infected
          auto parasites = person->all_clonal_parasite_populations()->parasites();
          auto size = parasites->size();
          if (size == 0) { continue; }

          // Check for the genotype interest, since we are tracking the absolute count of infected individuals once one
          // clone that matches has been found we can press on to the next person
          for (unsigned int infection = 0; infection < size; infection++) {
            auto parasite_population = (*parasites)[infection];
            if (parasite_population->genotype()->gene_expression()[LOCUS] == ALLELE) {
              count++;
              continue;
            }
          }
        }
      }
    }

    // Store the count for this location
    genotype_count[lookup[location]] += count;
  }

  // Counting is complete, start the update by getting the monthlydataid to use
  pqxx::connection* connection = pqxx_db::get_connection();
  pqxx::work db(*connection);
  pqxx::result results = db.exec(fmt::format(SELECT_MONTHLYDATAID, Model::MODEL->replicate()));
  auto id = results[0][0].as<int>();

  // Determine the offset to use for the updates
  auto offset = (aggregation == "D") ? SpatialData::get_instance().get_first_district() : 0;

  // Now prepare the query with the count updates
  std::string query;
  for (auto ndx = 0; ndx < genotype_count.size(); ndx++) {
    query.append(fmt::format(UPDATE_GENOTYPES, genotype_count[ndx], id, ndx + offset));
  }

  // Update the database, and clean up before returning
  db.exec(query);
  db.commit();
  connection->close();
  delete connection;
}

void GenotypeCarriersReporter::build_lookup(int replicate,  pqxx::connection* connection) {
  // District reporting just requires that we build a look-up where each cell is
  // mapped to the array index, and the array stores the district identification
  if (aggregation == "D") {
    auto offset = SpatialData::get_instance().get_first_district();
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      lookup.emplace_back(SpatialData::get_instance().get_district(loc) - offset);
    }
    lookup_allocation = SpatialData::get_instance().get_district_count();
    return;
  }

  // We must be doing a cellular count, which can be loaded from the database
  pqxx::work db(*connection);
  pqxx::result results = db.exec(fmt::format(SELECT_LOCATION, replicate));
  auto locations = Model::CONFIG->number_of_locations();
  for (auto ndx = 0; ndx < locations; ndx++) {
    lookup.emplace_back(results[ndx][0].as<int>());
  }
  lookup_allocation = (int)lookup.size();

  // Clear the transaction since we didn't do anything
  db.abort();
}