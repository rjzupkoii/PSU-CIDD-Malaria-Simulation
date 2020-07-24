/*
 * seasonal_info.cpp
 * 
 * Implement the loading of the seasonal_info YAML node.
 */ 

#include <cmath>

#include "CustomConfigItem.h"
#include "Config.h"
#include "GIS/AscFile.h"
#include "GIS/SpatialData.h"

void seasonal_info::clear() {
  value_.A.clear();
  value_.B.clear();
  value_.C.clear();
  value_.phi.clear();
  value_.min_value.clear();
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

  // Verify our node data
  if (!(seasonal_info_node["a"].size() == seasonal_info_node["phi"].size() == seasonal_info_node["min_value"].size())) {
    throw std::invalid_argument("seasonal_info nodes 'a', 'phi', and 'min_value' are not the same size.");
  }

  value_.enable = seasonal_info_node["enable"].as<bool>();
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
    
    // Verify our node data, note a single size
    if (!(seasonal_info_node["a"].size() == seasonal_info_node["phi"].size() == seasonal_info_node["min_value"].size())) {
        throw std::invalid_argument("seasonal_info nodes 'a', 'phi', and 'min_value' are not the same size.");
    }
    auto size = seasonal_info_node["a"].size();

    // Load the values based upon the raster data
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

    const auto period = node["period"].as<double>();
    auto B = 2 * M_PI / period;
    value_.B.push_back(B);

    const auto phi = node["phi"][index].as<float>();
    value_.phi.push_back(phi);

    auto C = -phi * B;
    value_.C.push_back(C);

    value_.min_value.push_back(node["min_value"][index].as<float>());
}
