/*
 * Spatial.h
 * 
 * Defitions of the thread-safe singleton pattern spatial class which manages the spatial aspects of the model from a high level. 
 */
#ifndef SPATIAL_H
#define SPATIAL_H

#include <string>

enum SpatialFileType {

};

class Spatial {

    private:
        Spatial() { }

        // Check the loaded spatial catalog for errors, returns true if there are errors
        bool check_catalog(std::string* errors);

    public:
        // Not supported by singleton.
        Spatial(Spatial const&) = delete;

        // Not supported by singleton.
        void operator=(Spatial const&) = delete;

        // Get a reference to the spatial object.
        static Spatial& get_instance() {
            static Spatial instance;
            return instance;
        }

        // Load the given raster file into the spatial catalog and assign the given label
        void load(std::string filename, SpatialFileType type);

};

#endif
