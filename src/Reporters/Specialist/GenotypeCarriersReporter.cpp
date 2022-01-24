/*
 * GenotypeCarriersReporter.cpp
 *
 * Implement the GenotypeCarriersReporter class.
 */
#include "GenotypeCarriersReporter.h"

#include <pqxx/pqxx>

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Reporters/Utility/PqxxHelpers.hxx"
#include "Model.h"
#include "easylogging++.h"

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

  // Clean up, note that a transaction created, so we need to manage it
  db.abort();
  connection->close();
  delete connection;

  // Inform the user that we are running, note the genotype
  LOG(INFO) << fmt::format("Absolute count of carriers of {} is being recorded to database.", GENOTYPE);
  VLOG(1) << "Replicate: " << replicate << ", Aggregation: " << aggregation;
}

void GenotypeCarriersReporter::monthly_report() {
  // TODO Flesh out this class
}