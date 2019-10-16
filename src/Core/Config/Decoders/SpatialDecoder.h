/*
 * SpatialDecoder.h
 * 
 * Define the SpatialDecoder which reads the raster_db
 * YAML node and converts it to the location_db concept
 * in the simulation.
 */
#ifndef SPATIALDECODER_H
#define SPATIALDECODER_H

#include <yaml-cpp/yaml.h>

class Config;

class SpatialDecoder {
    public:
        static bool decode(Config* config, const YAML::Node& node);

    private:
        // Disallow creation of object, static class
        SpatialDecoder() { }
};

#endif