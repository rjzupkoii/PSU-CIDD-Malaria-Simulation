/*
 * SpecialistReporterBase.cpp
 *
 * Implement the functionality of the extended SpecialistReporterBase class.
 */
#include "SpecialistReporterBase.h"

#include <pqxx/pqxx>

#include "Core/Config/Config.h"
#include "Model.h"

void SpecialistReporterBase::build_lookup(pqxx::connection *connection) {
  // Start by first checking to make sure we have a valid replicate
  int replicate = Model::MODEL->replicate();
  if (replicate == 0) {
    LOG(ERROR) << "Database replicate number does not appear to be set, unable to determine aggregation level";
    throw std::runtime_error("Missing database replicate number");
  }

  // Next check what the current aggregation level is
  pqxx::work db(*connection);
  auto result = db.exec(fmt::format(SELECT_AGGREGATION, replicate));
  if (result.empty()) {
    LOG(ERROR) << "Database did not return the aggregation level for the study";
    throw std::runtime_error("Aggregation level not returned");
  }
  aggregation = result[0][0].as<std::string>();

  if (aggregation == "D") {
    // District reporting just requires that we build a look-up where each cell is mapped to the array index,
    // and the array stores the district identification
    auto offset = SpatialData::get_instance().get_first_district();
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      lookup.emplace_back(SpatialData::get_instance().get_district(loc) - offset);
    }
    lookup_allocation = SpatialData::get_instance().get_district_count();

  } else if (aggregation == "C") {
    // We must be doing a cellular count, which can be loaded from the database
    pqxx::result results = db.exec(fmt::format(SELECT_LOCATION, replicate));
    auto locations = Model::CONFIG->number_of_locations();
    for (auto ndx = 0; ndx < locations; ndx++) {
      lookup.emplace_back(results[ndx][0].as<int>());
    }
    lookup_allocation = (int) lookup.size();

  } else {
    LOG(ERROR) << "Unknown aggregation level code: " << aggregation;
    throw std::runtime_error("Unknown aggregation code returned");
  }

  // Clean-up the transaction
  db.abort();
}
