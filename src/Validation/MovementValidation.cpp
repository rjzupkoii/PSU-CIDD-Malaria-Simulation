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
#include "Reporters/MovementReporter.h"
#include "Spatial/SpatialModel.h"
#include "Utility/MatrixWriter.hxx"

void MovementValidation::write_movement_data() {
  const std::string DISTANCES = "./dump/distances.csv";
  const std::string ODDS = "./dump/odds.csv";

  // Create the directory
  mkdir("./dump", 0777);

  // Get the distances matrix and dump it
  LOG(INFO) << "Dumping distance matrix to: " << DISTANCES;
  MatrixWriter<DoubleVector2>::write(Model::CONFIG->spatial_distance_matrix(), DISTANCES);

  // Setup the references and get the population
  const auto distance_matrix = Model::CONFIG->spatial_distance_matrix();
  const auto location_count = distance_matrix.size();
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