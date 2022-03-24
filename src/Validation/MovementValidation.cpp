/*
 * MovementValidation.cpp
 * 
 * Implement the methods required for movement validation.
 */
#include "MovementValidation.h"

#include <fmt/format.h>

#include "Core/Config/Config.h"
#include "easylogging++.h"
#include "Model.h"
#include "Reporters/Specialist/MovementReporter.h"
#include "Spatial/SpatialModel.hxx"
#include "GIS/SpatialData.h"
#include "Utility/MatrixWriter.hxx"

#define LOCATION_COUNT_CUTOFF 100

void MovementValidation::add_move(int individual, int source, int destination) { 
  // Record individual movement if need be
  auto& instance = get_instance();
  if (instance.individual_movement_) {
    instance.reporter->add_fine_move(individual, source, destination); 
    return;
  }

  // Record coarse cell movement if need be
  if (instance.cell_movement_) {
    instance.reporter->add_coarse_move(individual, source, destination);
    return;
  }

  // Return if there is nothing to do
  if (!instance.district_movement_) { return; }

  // Determine the districts
  auto& sd = SpatialData::get_instance();
  int source_district = sd.get_district(source);
  int destination_district = sd.get_district(destination);

  // Record the move
  instance.reporter->add_coarse_move(individual, source_district, destination_district);
}

void MovementValidation::write_movement_data() {
  const std::string DISTANCES = "./dump/distances.csv";
  const std::string ODDS = "./dump/odds.csv";

  // Before we do anything, check the size of the matrix
  const auto location_count = Model::CONFIG->spatial_distance_matrix().size();
  if (location_count > LOCATION_COUNT_CUTOFF) {
    LOG(WARNING) << "Location count of " << location_count << " exceeds maximum of " << LOCATION_COUNT_CUTOFF << " distances and odds not dumped.";
    return;
  }

  // Create the directory
  mkdir("./dump", 0777);

  // Get the distances matrix and dump it
  LOG(INFO) << "Dumping distance matrix to: " << DISTANCES;
  MatrixWriter<DoubleVector2>::write(Model::CONFIG->spatial_distance_matrix(), DISTANCES);

  // Setup the references and get the population
  const auto distance_matrix = Model::CONFIG->spatial_distance_matrix();
  const auto spatial = Model::CONFIG->spatial_model();
  IntVector residents_by_location(location_count, 0);
  for (std::size_t ndx = 0; ndx < location_count; ndx++) {
    residents_by_location[ndx] = Model::CONFIG->location_db()[ndx].population_size;
  }

  // Calculate the odds for each row, then write the data
  DoubleVector2 odds(location_count);  
  for (std::size_t ndx = 0; ndx < location_count; ndx++) {
    odds[ndx].resize(location_count, 0);
    odds[ndx] = spatial->get_v_relative_out_movement_to_destination(ndx, location_count, distance_matrix[ndx], residents_by_location);
  }

  LOG(INFO) << "Dumping odds matrix to: " << ODDS;
  MatrixWriter<DoubleVector2>::write(odds, ODDS);
} 