/*
 * seasonal_info.cpp
 * 
 * Implement the loading of the seasonal_info YAML node as well as the seasonal factor equation / algorihtm.
 */ 

#include <cmath>

#include "CustomConfigItem.h"
#include "Config.h"
#include "Constants.h"
#include "GIS/AscFile.h"
#include "GIS/SpatialData.h"
#include "Helpers/TimeHelpers.h"
#include "Model.h"

void seasonal_info::clear() {
  value_.A.clear();
  value_.B.clear();
  value_.phi.clear();
  value_.period.clear();
  value_.base.clear();
}

double seasonal_info::get_seasonal_factor(const date::sys_days &today, const int &location) {
  // If the seasonal factor has not been enabled, return one
  if (!Model::CONFIG->seasonal_info().enable) { return 1; } 

  // Note what day of the year it is
  int day = TimeHelpers::day_of_year(today);

  // Seasonal factor is determined by the algorithm:
  // 
  // multiplier = a + b * sin((2 * pi * (phi - t)) / period);
  // multiplier(multiplier < 0) = 0;
  // multiplier = base + multiplier;
  auto multiplier = Model::CONFIG->seasonal_info().A[location] + Model::CONFIG->seasonal_info().B[location] * 
    sin(2 * M_PI * (Model::CONFIG->seasonal_info().phi[location] - day) / Model::CONFIG->seasonal_info().period[location]);
  multiplier = (multiplier < 0) ? 0 : multiplier;
  multiplier += Model::CONFIG->seasonal_info().base[location];

  // Return the multiplier
  return multiplier;
}

void seasonal_info::set_value(const YAML::Node &node) {
  // Before doing anything, check to see if there is a raster
  if (node["raster"] && node["raster"].as<bool>()) {
      set_from_raster(node);
      return;
  }

  // Continue loading the YAML using the older approach
  auto seasonal_info_node = node[name_];
  clear();
    
  // Set if we are enabled or not
  value_.enable = seasonal_info_node["enable"].as<bool>();

  // Warn the user if enough nodes were not provided
  if (value_.enable && seasonal_info_node["a"].size() > 1 && seasonal_info_node["a"].size() < config_->number_of_locations()) {
    LOG(WARNING) << fmt::format("Only {} seasonal settings provided, but {} are needed for all locations", seasonal_info_node["a"].size(), config_->number_of_locations());
  }

  for (auto i = 0ul; i < config_->number_of_locations(); i++) {
    auto input_loc = seasonal_info_node["a"].size() < config_->number_of_locations() ? 0 : i;
    set_seasonal_period(seasonal_info_node, input_loc);
  }
}

void seasonal_info::set_from_raster(const YAML::Node &node) {
    // Get the raster data and make sure it is valid
    AscFile* raster = SpatialData::get_instance().get_raster(SpatialData::SpatialFileType::Ecoclimatic);
    if (raster == nullptr) {
        throw std::invalid_argument("seasonal_info raster flag set without eco-climatic raster loaded.");
    }

    // Prepare to run
    auto seasonal_info_node = node[name_];
    clear();
    
    // Load the values based upon the raster data
    auto size = seasonal_info_node["a"].size();
    value_.enable = seasonal_info_node["enable"].as<bool>();
    for (int row = 0; row < raster->NROWS; row++) {
        for (int col = 0; col < raster->NCOLS; col++) {
            // Pass if we have no data here
            if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }

            // Verify the index
            int index = static_cast<int>(raster->data[row][col]);
            if (index < 0) { throw std::out_of_range(fmt::format("Raster value at row: {}, col: {} is zero.", row, col)); }
            if (index > (size - 1)) { throw std::out_of_range(fmt::format("Raster value at row: {}, col: {} exceeds bounds of {}.", row, col, size)); }

            // Set the seasonal period
            set_seasonal_period(seasonal_info_node, index);
        }
    }
}

void seasonal_info::set_seasonal_period(const YAML::Node &node, int index) {
  value_.A.push_back(node["a"][index].as<double>());
  value_.B.push_back(node["b"][index].as<double>());
  value_.phi.push_back(node["phi"][index].as<double>());
  value_.period.push_back(node["period"][index].as<double>());
  value_.base.push_back(node["base"][index].as<double>());
}
