/*
 * SpaitalData.cpp
 * 
 *
 */
#include "SpatialData.h"

#include <fmt/format.h>
#include <stdexcept>

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

bool SpatialData::check_catalog(std::string* errors) {
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
        if (data[ndx]->CELLSIZE != reference->CELLSIZE) { *errors += "mismatched CELLSIZE;"; }
        if (data[ndx]->NCOLS != reference->NCOLS) { *errors += "mismatched NCOLS;"; }
        if (data[ndx]->NODATA_VALUE != reference->NODATA_VALUE) { *errors += "mismatched NODATA_VALUE;"; }
        if (data[ndx]->NROWS != reference->NROWS) { *errors += "mismatched NROWS;"; }
        if (data[ndx]->XLLCENTER != reference->XLLCENTER) { *errors += "mismatched XLLCENTER;"; }
        if (data[ndx]->XLLCORNER != reference->XLLCORNER) { *errors += "mismatched XLLCORNER;"; }
        if (data[ndx]->YLLCENTER != reference->YLLCENTER) { *errors += "mismatched YLLCENTER;"; }
        if (data[ndx]->YLLCORNER != reference->YLLCORNER) { *errors += "mismatched YLLCORNER;"; }
    }

    // Return true if errors were found
    return (!errors->empty());
}

void SpatialData::load(std::string filename, SpatialFileType type) {
    // Check to see if something has already been loaded
    if (data[type] != nullptr) { delete data[type]; }

    // Load the data and set the reference
    AscFile* file = AscFileManager::read(filename);
    data[type] = file;
}

void SpatialData::refresh() {
    // TODO Write this function
}

void SpatialData::write(std::string filename, SpatialFileType type) {
    // Check to make sure there is something to write
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("No data for spatial file type {}, write file {}", type, filename)); 
    }

    // Write the data
    AscFileManager::write(data[type], filename);
}