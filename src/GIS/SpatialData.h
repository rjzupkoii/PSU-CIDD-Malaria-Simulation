/*
 * SpatialData.h
 * 
 * Defitions of the thread-safe singleton pattern spatial class which manages the spatial aspects of the model from a high level. 
 */
#ifndef SPATIALDATA_H
#define SPATIALDATA_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "AscFile.h"

class SpatialData {
    public:
        enum SpatialFileType {
            // Only use the data to define the model's location listing
            RawLocations,

            // Entomological Inoculation Rates (EIR)
            EIR,

            // Population data
            Population,

            // Number of sequential items in the type
            Count
        };

    private:
        // Array of the ASC file data, use SpatialFileType as the index
        AscFile** data;

        // Flag to indicate if data has been loaded since the last time it was checked
        bool dirty;

        // Constructor
        SpatialData();

        // Deconstructor
        ~SpatialData();

        // Check the loaded spatial catalog for errors, returns true if there are errors
        bool check_catalog(std::string& errors);

        // Generate the locations for the location_db
        void generate_locations();

    public:
        // Not supported by singleton.
        SpatialData(SpatialData const&) = delete;

        // Not supported by singleton.
        void operator=(SpatialData const&) = delete;

        // Get a reference to the spatial object.
        static SpatialData& get_instance() {
            static SpatialData instance;
            return instance;
        }

        // Clears all of the loaded data
        void clear();

        // Load the given raster file into the spatial catalog and assign the given label
        void load(std::string filename, SpatialFileType type);

        // Parse the YAML node provided to extract all of the relevent information for the simulation
        bool parse(const YAML::Node &node);

        // Refresh the data from the model (i.e., Location DB) to the spaital data
        void refresh();

        // Write the current spaital data to the filename and path indicated, output will be an ASC file
        void write(std::string filename, SpatialFileType type);

};

#endif
