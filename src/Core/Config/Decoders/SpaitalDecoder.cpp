/*
 * SpatialDecoder.cpp
 * 
 * Implement SpatialDecoder methods.
 */
#include "SpatialDecoder.h"

#include "easylogging++.h"
#include "GIS/SpatialData.h"

bool SpatialDecoder::decode(Config* config, const YAML::Node& node) {

    // First, start by attempting to load any rasters
    VLOG(1) << "Scanning for rasters";
    SpatialData& instance = SpatialData::get_instance();
    if (node["location_raster"]) {
        instance.load(node["location_raster"].as<std::string>(), SpatialData::SpatialFileType::RawLocations);
    }
    if (node["beta_raster"]) {
        instance.load(node["beta_raster"].as<std::string>(), SpatialData::SpatialFileType::EIR);
    }
    if (node["population_raster"]) {
        instance.load(node["population_raster"].as<std::string>(), SpatialData::SpatialFileType::Population);
    }

    // Now convert the rasters into the locattion space

    // TODO ...
}