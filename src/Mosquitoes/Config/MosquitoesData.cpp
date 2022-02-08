/*
 * MosquitoesData.cpp
 * 
 * Implementation of MosquitoesData functions.
 */
#include "MosquitoesData.h"

#include <fmt/format.h>
#include <math.h>
#include <stdexcept>

#include "Core/Config/Config.h"
#include "easylogging++.h"
#include "Model.h"

MosquitoesData::MosquitoesData() {
    data = new AscFile*[MosquitoesFileType::Count]();
}

MosquitoesData::~MosquitoesData() {
    if (data == nullptr) { return; }
    for (auto ndx = 0; ndx!= MosquitoesFileType::Count; ndx++) {
        if (data[ndx] != nullptr) {
            delete data[ndx];
        }
    }
    delete [] data;
}

bool MosquitoesData::check_catalog(std::string& errors) {
    // Reference parameters
    AscFile* reference;

    // Load the parameters from the first entry
    auto ndx = 0;
    for (; ndx != MosquitoesFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        reference = data[ndx];
        break;
    }

    // If we hit the end, then there must be nothing loaded
    if (ndx == MosquitoesFileType::Count) { return true; }

    // Check the remainder of the enteries, do this by validating the header
    for (; ndx != MosquitoesFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        if (data[ndx]->CELLSIZE != reference->CELLSIZE) { errors += "mismatched CELLSIZE;"; }
        if (data[ndx]->NCOLS != reference->NCOLS) { errors += "mismatched NCOLS;"; }
        if (data[ndx]->NODATA_VALUE != reference->NODATA_VALUE) { errors += "mismatched NODATA_VALUE;"; }
        if (data[ndx]->NROWS != reference->NROWS) { errors += "mismatched NROWS;"; }
        if (data[ndx]->XLLCENTER != reference->XLLCENTER) { errors += "mismatched XLLCENTER;"; }
        if (data[ndx]->XLLCORNER != reference->XLLCORNER) { errors += "mismatched XLLCORNER;"; }
        if (data[ndx]->YLLCENTER != reference->YLLCENTER) { errors += "mismatched YLLCENTER;"; }
        if (data[ndx]->YLLCORNER != reference->YLLCORNER) { errors += "mismatched YLLCORNER;"; }
    }

    // Set the dirty flag based upon the errors, return the result
    auto has_errors = !errors.empty();
    dirty = has_errors;
    return has_errors;
}

MosquitoesData::RasterInformation MosquitoesData::get_raster_header() {
    RasterInformation results;
    for (auto ndx = 0; ndx < MosquitoesFileType::Count; ndx++) {
        if (data[ndx] != nullptr) {
            results.number_columns = data[ndx]->NCOLS;
            results.number_rows = data[ndx]->NROWS;
            results.x_lower_left_corner = data[ndx]->XLLCORNER;
            results.y_lower_left_corner = data[ndx]->YLLCORNER;
            results.cellsize = data[ndx]->CELLSIZE;
            break;
        }
    }
    return results;
}

bool MosquitoesData::has_raster() {
    for (auto ndx = 0; ndx != MosquitoesFileType::Count; ndx++) {
        if (data[ndx] != nullptr) { return true; }
    }
    return false;
}

void MosquitoesData::load(std::string filename, MosquitoesFileType type) {
    // Check to see if something has already been loaded
    if (data[type] != nullptr) { delete data[type]; }

    // Load the data and set the reference
    VLOG(0) << "[MosquitoesData] Loading " << filename;
    AscFile* file = AscFileManager::read(filename);
    data[type] = file;

    // The data needs to be checked
    dirty = true;
}

void MosquitoesData::load_raster(MosquitoesFileType type) {
    // Verify that that the raster data has been loaded
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("{} called without raster, type id: {}", __FUNCTION__, type));
    }
    auto *values = data[type];

    // Grab a reference to the mosquitoes to work with
    auto& prmc = Model::CONFIG->prmc();
    auto count = Model::CONFIG->number_of_locations();

    // Iterate through the raster and locations to set the value
    auto id = 0;
    for (auto row = 0; row < values->NROWS; row++) {
        for (auto col = 0; col < values->NCOLS; col++) {

            if (values->data[row][col] == values->NODATA_VALUE) { continue; }

            if (id > count) {
                throw std::runtime_error(fmt::format("Raster misalignment: pixel count exceeds {} expected while loading raster type {}", count, type));
            }

            switch (type) {
                case MosquitoesFileType::InteruptedFeedingRate:
                    prmc->addInterruptedFeedingRateByLocation(id,static_cast<double>(values->data[row][col]));
                    break;
                default: 
                    throw std::runtime_error(fmt::format("{} called with invalid raster, type id: {}", __FUNCTION__, type));
            }
            id++;
        }
    }

    // When we are done the last id value should match the number of locations, accounting for zero indexing
    if ((unsigned)(id) != count) {
        throw std::runtime_error(fmt::format("Raster misalignment: found {} pixels, expected {} while loading raster type {}", id, count, type));
    }

    // Log the updates
//    VLOG(0) << "[MosquitoesData] Loaded values from raster file, type id: " << type;
}

void MosquitoesData::load_files(const YAML::Node &node) {
    VLOG(0) << "[MosquitoesData] Loading node: " << node;
    if (node[TYPE_RASTER]) {
        load(node[TYPE_RASTER].as<std::string>(), MosquitoesData::MosquitoesFileType::Type);
    }
    if (node[INTERRUPTED_FEEDING_RATE_RASTER]) {
        load(node[INTERRUPTED_FEEDING_RATE_RASTER].as<std::string>(), MosquitoesData::MosquitoesFileType::InteruptedFeedingRate);
    }
}

bool MosquitoesData::parse(const YAML::Node &node) {

    // First, start by attempting to load any rasters
    load_files(node);

    // Set the cell size
    cell_size = node["cell_size"].as<float>();

    auto& prmc = Model::CONFIG->prmc();
    prmc->set_size(node["prmc_size"].as<int>());
    prmc->set_starting_date(node["prmc_starting_date"].as<date::year_month_day>());
    prmc->set_ending_date(node["prmc_ending_date"].as<date::year_month_day>());
    prmc->set_generate_daily_report(node["daily_report"].as<bool>());
    // Now convert the rasters into the location space
    refresh();

    // Load completed successfully
    parse_complete();
    return true;
}

void MosquitoesData::parse_complete() {
    if (data[MosquitoesFileType::InteruptedFeedingRate] != nullptr) {
        delete data[MosquitoesFileType::InteruptedFeedingRate];
        data[MosquitoesFileType::InteruptedFeedingRate] = nullptr;
    }
}

void MosquitoesData::refresh() {
    // Check to make sure our data is OK
    std::string errors;
    if (dirty && check_catalog(errors)) {
        throw std::runtime_error(errors);
    }
    
    // Mosquito types must be loaded before population
    if (data[MosquitoesFileType::InteruptedFeedingRate] != nullptr) {
        load_raster(MosquitoesFileType::InteruptedFeedingRate);
    }
}

void MosquitoesData::write(std::string filename, MosquitoesFileType type) {
    // Check to make sure there is something to write
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("No data for Mosquitoes file type {}, write file {}", type, filename));
    }

    // Write the data
    AscFileManager::write(data[type], filename);
}