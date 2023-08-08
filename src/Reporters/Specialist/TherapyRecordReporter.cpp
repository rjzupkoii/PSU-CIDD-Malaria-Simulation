/*
 * TherapyRecordReporter.cpp
 *
 * Implement the TherapyRecordReporter class.
 */
#include "TherapyRecordReporter.h"

#include <pqxx/pqxx>

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Reporters/Utility/PqxxHelpers.h"

void TherapyRecordReporter::initialize(int job_number, std::string path) {
  // Query to verify that the database has been properly configured
  pqxx::connection* connection = pqxx_db::get_connection();
  pqxx::work db(*connection);
  pqxx::result result = db.exec(CHECK_EXISTENCE);
  auto flag = result[0][0].as<bool>();

  // If the flag is set to false then the table doesn't exist
  if (!flag) {
    LOG(ERROR) << "Database not configured correctly for Therapy Record Reporter, missing the \'sim.therapyrecord\' table";
    throw std::runtime_error("Missing Therapy Record table");
  }

  // Use the connection to build the lookup table
  build_lookup(connection);

  // Clean-up before exiting
  db.abort();
  connection->close();
  delete connection;
}

void TherapyRecordReporter::monthly_report() {
  // Start the update by getting the monthlydataid to use
  pqxx::connection* connection = pqxx_db::get_connection();
  pqxx::work db(*connection);
  pqxx::result results = db.exec(fmt::format(SELECT_MONTHLYDATAID, Model::MODEL->replicate()));
  auto id = results[0][0].as<int>();

  // If we are aggregating at the district level, defer to the function which will clean up the connection
  if (aggregation == "D") {
    db.abort();
    district_report(id, connection);
    return;
  }

  // Otherwise build the query to insert the data
  std::string query(INSERT_THERAPY_PREFIX);
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (auto therapy = 0; therapy < Model::CONFIG->therapy_db().size(); therapy++) {
      query.append(fmt::format(INSERT_THERAPY_ROW,
                               id, lookup[location], therapy,
                               Model::DATA_COLLECTOR->monthly_treatment_success_by_location_therapy()[location][therapy],
                               Model::DATA_COLLECTOR->monthly_treatment_failure_by_location_therapy()[location][therapy],
                               Model::DATA_COLLECTOR->monthly_treatment_complete_by_location_therapy()[location][therapy]));
    }
  }
  query[query.length() - 1] = ';';

  // Update the database, clean up before returning
  db.exec(query);
  db.commit();
  connection->close();
  delete connection;
}

void TherapyRecordReporter::district_report(int id, pqxx::connection *connection) {
  // Cache some values
  auto therapies = Model::CONFIG->therapy_db().size();
  auto first_index = SpatialData::get_instance().get_first_district();

  // Setup our storage
  std::vector<std::vector<int>> complete(lookup_allocation, std::vector<int>(therapies, 0));
  std::vector<std::vector<int>> failure(lookup_allocation, std::vector<int>(therapies, 0));
  std::vector<std::vector<int>> success(lookup_allocation, std::vector<int>(therapies, 0));

  // Retrieve the data from the model data collector
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (auto therapy = 0; therapy < therapies; therapy++) {
      complete[lookup[location]][therapy] += Model::DATA_COLLECTOR->monthly_treatment_complete_by_location_therapy()[location][therapy];
      failure[lookup[location]][therapy] += Model::DATA_COLLECTOR->monthly_treatment_failure_by_location_therapy()[location][therapy];
      success[lookup[location]][therapy] += Model::DATA_COLLECTOR->monthly_treatment_success_by_location_therapy()[location][therapy];
    }
  }

  // Now prepare the query with the count updates
  std::string query(INSERT_THERAPY_PREFIX);
  for (auto location = 0; location < lookup_allocation; location++) {
    for (auto therapy = 0; therapy < therapies; therapy++) {
      query.append(fmt::format(INSERT_THERAPY_ROW,
                               id, location + first_index, therapy,
                               success[location][therapy], failure[location][therapy], success[location][therapy]));
    }
  }
  query[query.length() - 1] = ';';

  // Update the database and clean-up before returning
  pqxx::work db(*connection);
  db.exec(query);
  db.commit();
  connection->close();
  delete connection;
}


