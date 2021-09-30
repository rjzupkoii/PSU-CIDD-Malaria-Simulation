/*
 * SeasonalEquation.cpp
 *
 * Implement the equation based seasonal model.
 */
#include "SeasonalInfo.h"

#include <cmath>

#include "Core/Config/CustomConfigItem.h"
#include "Core/Config/Config.h"
#include "Constants.h"
#include "Helpers/TimeHelpers.h"
#include "Model.h"

SeasonalEquation* SeasonalEquation::build(const YAML::Node &node, Config* config) {
  // Prepare the object to be returned
  auto value = new SeasonalEquation();

  // Before doing anything, check to see if there is a raster
  if (node["raster"] && node["raster"].as<bool>()) {
    value->set_from_raster(node);
    return value;
  }

  // Check to make sure the nodes exist
  if (node["base"].IsNull()  || node["a"].IsNull() || node["b"].IsNull() || node["phi"].IsNull()) {
    throw std::invalid_argument("One or more of the seasonality parameters is missing.");
  }
  if (node["base"].size() == 0  || node["a"].size() == 0 || node["b"].size() == 0 || node["phi"].size() == 0) {
    throw std::invalid_argument("One or more of the seasonality parameters is an empty array.");
  }

  // Warn the user if enough nodes were not provided
  if (node["a"].size() > 1 && node["a"].size() < config->number_of_locations()) {
    LOG(WARNING) << fmt::format("Only {} seasonal settings provided, but {} are needed for all locations", node["a"].size(), config->number_of_locations());
  }

  // Set the values from the array and return
  for (auto i = 0ul; i < config->number_of_locations(); i++) {
    auto input_loc = node["a"].size() < config->number_of_locations() ? 0 : i;
    value->set_seasonal_period(node, input_loc);
  }
  return value;
}

double SeasonalEquation::get_seasonal_factor(const date::sys_days &today, const int &location) {
  // Note what day of the year it is
  int day = TimeHelpers::day_of_year(today);

  // Seasonal factor is determined by the algorithm:
  //
  // multiplier = base + (a * sin⁺(b * pi * (t - phi) / 365))
  auto multiplier = A[location] * sin(B[location] * M_PI * (day - phi[location]) / Constants::DAYS_IN_YEAR());
  multiplier = (multiplier < 0) ? 0 : multiplier;
  multiplier += base[location];

  // Return the multiplier
  return multiplier;
}

// Set the values based upon the contents of a raster file.
void SeasonalEquation::set_from_raster(const YAML::Node &node) {
  // Get the raster data and make sure it is valid
  AscFile* raster = SpatialData::get_instance().get_raster(SpatialData::SpatialFileType::Ecoclimatic);
  if (raster == nullptr) {
    throw std::invalid_argument("seasonal_info raster flag set without eco-climatic raster loaded.");
  }

  // Prepare to run
  LOG(INFO) << "Setting seasonal_info using raster data.";

  // Load the values based upon the raster data
  auto size = node["a"].size();
  for (int row = 0; row < raster->NROWS; row++) {
    for (int col = 0; col < raster->NCOLS; col++) {
      // Pass if we have no data here
      if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }

      // Verify the index
      int index = static_cast<int>(raster->data[row][col]);
      if (index < 0) { throw std::out_of_range(fmt::format("Raster value at row: {}, col: {} is zero.", row, col)); }
      if (index > (size - 1)) { throw std::out_of_range(fmt::format("Raster value at row: {}, col: {} exceeds bounds of {}.", row, col, size)); }

      // Set the seasonal period
      set_seasonal_period(node, index);
    }
  }

  // Update the reference values as well
  for (std::size_t ndx = 0; ndx < size; ndx++) {
    reference_base.push_back(node["base"][ndx].as<double>());
    reference_A.push_back(node["a"][ndx].as<double>());
    reference_B.push_back(node["b"][ndx].as<double>());
    reference_phi.push_back(node["phi"][ndx].as<double>());
  }
}

// Set the period for a single location given the index
void SeasonalEquation::set_seasonal_period(const YAML::Node &node, unsigned long index) {
  base.push_back(node["base"][index].as<double>());
  A.push_back(node["a"][index].as<double>());
  B.push_back(node["b"][index].as<double>());
  phi.push_back(node["phi"][index].as<double>());
}

// Update the seasonality of the equation from the current to the new one.
void SeasonalEquation::update_seasonality(int from, int to) {
  for (auto ndx = 0; ndx < base.size(); ndx++) {
    if (base[ndx] == reference_base[from] && A[ndx] == reference_A[from] && B[ndx] == reference_B[from] && phi[ndx] == reference_phi[from]) {
      base[ndx] = reference_base[to];
      A[ndx] = reference_A[to];
      B[ndx] = reference_B[to];
      phi[ndx] = reference_phi[to];
    }
  }
}
