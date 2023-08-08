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
#include "Reporters/Utility/PqxxHelpers.h"

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

  // Use the connection to build the lookup table
  build_lookup(connection);

  // Clean up, note that a transaction created, so we need to manage it
  db.abort();
  connection->close();
  delete connection;

  // Inform the user that we are running, note the genotype
  LOG(INFO) << fmt::format("Absolute count of carriers of {} (locus {}, allele {}) is being recorded to database.",
                           GENOTYPE, LOCUS, ALLELE);
  VLOG(1) << "Replicate: " << Model::MODEL->replicate() << ", Aggregation: " << aggregation;
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
