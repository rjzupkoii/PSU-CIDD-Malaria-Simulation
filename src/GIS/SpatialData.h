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
            Locations,

            // Transimission intensity, linked to the Entomological Inoculation Rates (EIR)
            Beta,

            // Population data
            Population,

            // District location
            Districts,

            // Number of sequential items in the type
            Count
        };

        struct RasterInformation {
            // Flag to indicate the value has not been set yet
            static const int NOT_SET = -1;

            // The number of columns in the raster
            int number_columns = NOT_SET;

            // The number of rows in the raster
            int number_rows = NOT_SET;

            // The lower-left X coordinate of the raster
            double x_lower_left_corner = NOT_SET;

            // The lower-left Y coordinate of the raster
            double y_lower_left_corner = NOT_SET;

            // The size of the cell, typically in meters
            int cellsize = NOT_SET;
        };

    private:
        const std::string BETA_RASTER = "beta_raster";
        const std::string DISTRICT_RASTER = "district_raster";
        const std::string LOCATION_RASTER = "location_raster";
        const std::string POPULATION_RASTER = "population_raster";

        // Array of the ASC file data, use SpatialFileType as the index
        AscFile** data;

        // Flag to indicate if data has been loaded since the last time it was checked
        bool dirty;

        // The size of the cells in the raster, the units shouldn't matter, but this was
        // written when we were using 5x5 km cells
        float cell_size = 0;

        // Count of district loaded in the map, default zero, lazy initialization to actual value when 
        // requested the first time
        int district_count = 0;

        // Constructor
        SpatialData();

        // Deconstructor
        ~SpatialData();

        // Check the loaded spatial catalog for errors, returns true if there are errors
        bool check_catalog(std::string& errors);

        // Generate the locations for the location_db
        void generate_locations();

        // Load the beta values into the location_db
        void load_beta();

        // Load the popuation values into the location_db
        void load_population();

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

        // Return the raster header or the default structure if no raster are loaded
        RasterInformation get_raster_header();

        // Return true if any raster file has been loaded, false otherwise
        bool has_raster();

        // Return true if a raster file has been loaded, false otherwise
        bool has_raster(SpatialFileType type) { return data[type] != nullptr; }

        // Generate the Euclidian distances for the location_db
        void generate_distances();

        // Get the district id that corresponds to the cell id
        int get_district(int location);

        // Get the count of districts loaded, or -1 if they have not been loaded
        int get_district_count();

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
