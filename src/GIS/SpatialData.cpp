/*
 * SpaitalData.cpp
 * 
 *
 */
#include "SpatialData.h"

#include <fmt/format.h>
#include <stdexcept>

#include "Core/Config/Config.h"
#include "Model.h"

SpatialData::SpatialData() {
    data = new AscFile*[SpatialFileType::Count]();
}

SpatialData::~SpatialData() {
    if (data == nullptr) { return; }
    for (auto ndx = 0; ndx!= SpatialFileType::Count; ndx++) {
        if (data[ndx] != nullptr) {
            delete data[ndx];
        }
    }
    delete [] data;
}

bool SpatialData::check_catalog(std::string& errors) {
    // Reference parameters
    AscFile* reference;

    // Load the parameters from the first entry
    auto ndx = 0;
    for (; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        reference = data[ndx];
        break;
    }

    // If we hit the end, then there must be nothing loaded
    if (ndx == SpatialFileType::Count) { return true; }

    // Check the remainder of the enteries, do this by validating the header
    for (; ndx != SpatialFileType::Count; ndx++) {
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

    std::cout << errors << std::endl;

    // Set the dirty flag based upon the errors, return the result
    auto has_errors = !errors.empty();
    dirty = has_errors;
    return has_errors;
}

void SpatialData::generate_locations() {
    // Reference parameters
    AscFile* reference;

    // Scan for a ASC file to use to generate with
    auto ndx = 0;
    for (; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        reference = data[ndx];
        break;
    }

    // If we didn't find one, return
    // NOTE This might be an error case - review this code later
    if (ndx == SpatialFileType::Count) { return; }

    // Start by overallocating the location_db
    auto& db = Model::CONFIG->location_db();
    db.reserve(reference->NROWS * reference->NCOLS);

    // Scan the data and insert fields with a value
    for (auto row = 0; row < reference->NROWS; row++) {
        for (auto col = 0; col < reference->NCOLS; col++) {
            if (reference->data[row][col] == reference->NODATA_VALUE) { continue; }
            db.emplace_back(reference->data[row][col], row, col, 0);
        }
    }

    // It's likely we over allocated, allow some space to be reclaimed
    db.shrink_to_fit();
}

void SpatialData::load(std::string filename, SpatialFileType type) {
    // Check to see if something has already been loaded
    if (data[type] != nullptr) { delete data[type]; }

    // Load the data and set the reference
    AscFile* file = AscFileManager::read(filename);
    data[type] = file;

    // The data needs to be checked
    dirty = true;
}

void SpatialData::refresh() {
    // Check to make sure our data is OK
    std::string errors;
    if (dirty && check_catalog(errors)) {
        throw std::runtime_error(errors);
    }

    // We have data and we know that it should be located in the same geographic space,
    // so now we can now refresh the location_db
    auto& db = Model::CONFIG->location_db();
    if (db.size() == 0) {
        std::cout << "Generating!" << std::endl;
        generate_locations();
        std::cout << "done!" << std::endl;
    }

}

void SpatialData::write(std::string filename, SpatialFileType type) {
    // Check to make sure there is something to write
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("No data for spatial file type {}, write file {}", type, filename)); 
    }

    // Write the data
    AscFileManager::write(data[type], filename);
}