/*
 * MosquitoesData.h
 * 
 * Definitions of the thread-safe singleton pattern mosquitoes class which manages the mosquito aspects of the model from a high level.
 */
#ifndef MOSQUITOESDATA_H
#define MOSQUITOESDATA_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "GIS/AscFile.h"

class MosquitoesData {
    public:
        enum MosquitoesFileType {
            // Population data
            Population,
            //Type data
            Type,
            //Infected bite rate data
//            InfectedBiteRate,
            //Interupted feeding data
            InteruptedFeedingRate,

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
        const std::string POPULATION_RASTER = "population_raster";
        const std::string TYPE_RASTER = "type_raster";
//        const std::string BITE_RATE_RASTER = "infected_bite_rate_raster";
        const std::string INTERRUPTED_FEEDING_RATE_RASTER = "interrupted_feeding_rate_raster";

        // Array of the ASC file data, use MosquitoesFileType as the index
        AscFile** data;

        // Flag to indicate if data has been loaded since the last time it was checked
        bool dirty;

        // The size of the cells in the raster, the units shouldn't matter, but this was
        // written when we were using 5x5 km cells
        float cell_size = 0;

        // Constructor
        MosquitoesData();

        // Deconstructor
        ~MosquitoesData();

        // Check the loaded Mosquitoes catalog for errors, returns true if there are errors
        bool check_catalog(std::string& errors);

        // Load the given raster file into the Mosquitoes catalog and assign the given label
        void load(std::string filename, MosquitoesFileType type);

        // Load all of the Mosquitoes data from the node
        void load_files(const YAML::Node &node);

        // Load the raster indicated into the location_db; works with betas and probability of treatment
        void load_raster(MosquitoesFileType type);

        // Perform any clean-up operations after parsing the YAML file is complete
        void parse_complete();

    public:
        // Not supported by singleton.
        MosquitoesData(MosquitoesData const&) = delete;

        // Not supported by singleton.
        void operator=(MosquitoesData const&) = delete;

        // Get a reference to the Mosquitoes object.
        static MosquitoesData& get_instance() {
            static MosquitoesData instance;
            return instance;
        }

        // Return the raster header or the default structure if no raster are loaded
        RasterInformation get_raster_header();

        // Return true if any raster file has been loaded, false otherwise
        bool has_raster();

        // Return true if a raster file has been loaded, false otherwise
        bool has_raster(MosquitoesFileType type) { return data[type] != nullptr; }

        // Get a reference to the AscFile raster, may be a nullptr
        AscFile* get_raster(MosquitoesFileType type) { return data[type]; }

        // Parse the YAML node provided to extract all of the relevent information for the simulation
        bool parse(const YAML::Node &node);

        // Refresh the data from the model (i.e., Location DB) to the spaital data
        void refresh();

        // Write the current spaital data to the filename and path indicated, output will be an ASC file
        void write(std::string filename, MosquitoesFileType type);

};

#endif
