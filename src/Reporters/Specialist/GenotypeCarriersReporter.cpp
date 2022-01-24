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

  // Clean up, note that a transaction created, so we need to manage it
  db.abort();
  connection->close();
  delete connection;

  // If nothing was returned, throw an error
  if (result.empty()) {
    LOG(ERROR) << "Database not configured correctly for Genotype Carriers Reporter, missing \'sim.monthlysitedata.genotypecarriers\'";
    throw std::runtime_error("Missing Genotype Carriers Reporter column");
  }

  // Inform the user that we are running, note the genotype
  LOG(INFO) << fmt::format("Absolute count of carriers of {} is being recorded to database.", GENOTYPE);
}

void GenotypeCarriersReporter::monthly_report() {

}