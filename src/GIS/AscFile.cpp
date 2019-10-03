/*
 *
 */
#include "AscFile.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

AscFile::~AscFile() {
    if (data == nullptr) { return; } 

    // Free the allocated memory
    for (auto ndx = 0; ndx < NROWS; ndx++) {
        delete [] data[ndx];
    }
    delete [] data;
}

// Check that the contents fo the ASC file are correct. Returns TRUE if any errors are found, which 
// are enumerated in the string provided.
bool AscFileManager::checkAscFile(AscFile* file, std::string* errors) {
    
    // Check values that must be set
    if (file->NCOLS == AscFile::NOT_SET) {
        *errors += "Number of columns is not set.";
    }
    if (file->NROWS == AscFile::NOT_SET) {
        *errors += "Number of rows is not set.";
    }
    if (file->CELLSIZE == AscFile::NOT_SET) {
        *errors += "Cell size is not set.";
    }

    // The coordinate to fix the raster should either be the lower-left, or the center
    if (file->XLLCENTER == AscFile::NOT_SET && file->YLLCENTER == AscFile::NOT_SET &&
        file->XLLCORNER == AscFile::NOT_SET && file->YLLCORNER == AscFile::NOT_SET) {
        *errors += "No location provided for raster coordinate.";
    }
    if (file->XLLCENTER != AscFile::NOT_SET && file->YLLCENTER != AscFile::NOT_SET &&
        file->XLLCORNER != AscFile::NOT_SET && file->YLLCORNER != AscFile::NOT_SET) {
        *errors += "Conflicting raster coordinates.";
    }

    // Return true if errors were found
    return (!errors->empty());
}

// Read the indicated file from disk, caller is responsible for checking if 
// data is integer or floating point.
AscFile* AscFileManager::read(std::string fileName) {
    // Treat the struct as POD
    AscFile* results = new AscFile();

    // Open and read the first six lines of the header
    std::string field, value;
    std::ifstream in(fileName);
    for (auto ndx = 0; ndx < 6; ndx++) {

        // Read the field and value, cast to upper case
        in >> field >> value;
        std::transform(field.begin(), field.end(), field.begin(), ::toupper);

        // Store the values
        if (field == "NCOLS") {
            results->NCOLS = std::stoi(value);
        } else if (field == "NROWS") {
            results->NROWS = std::stoi(value);
        } else if (field == "XLLCENTER") {
            results->XLLCENTER = std::stod(value);
        } else if (field == "YLLCENTER") {
            results->YLLCENTER = std::stod(value);
        } else if (field == "XLLCORNER") {
            results->XLLCORNER = std::stod(value);
        } else if (field == "YLLCORNER") {
            results->YLLCORNER = std::stod(value);
        } else if (field == "CELLSIZE") {
            results->CELLSIZE = std::stod(value);
        } else if (field == "NODATA_VALUE") {
            results->NODATA_VALUE = std::stod(value);
        }
    }

    // Check the header to make sure it is valid
    std::string* errors = new std::string();
    if (checkAscFile(results, errors)) {
        throw std::runtime_error(*errors);
    }
    delete errors;

    // Allocate the memory and read the remainder of the actual raster data
    results->data = new double*[results->NROWS];
    for (auto ndx = 0; ndx < results->NROWS; ndx++) {
        results->data[ndx] = new double[results->NCOLS];
    }

    // Remainder of the file is the actual raster data
    for (auto ndx = 0; ndx < results->NROWS; ndx++) {
        for (auto ndy = 0; ndy < results->NCOLS; ndy++) {
            in >> value;
            results->data[ndx][ndy] = std::stod(value);
        }
    }

    // Clean-up and return
    in.close();
    return results;
}

// 
void AscFileManager::write(AscFile* file, std::string fileName, int precision) {

    // Open the file for writing
    std::ofstream out(fileName);

    // Write the header
    out << std::setprecision(16) << std::left
        << std::setw(HEADER_WIDTH) << "ncols" << file->NCOLS << std::endl
        << std::setw(HEADER_WIDTH) << "nrows" << file->NROWS << std::endl
        << std::setw(HEADER_WIDTH) << "xllcorner" << file->XLLCORNER << std::endl
        << std::setw(HEADER_WIDTH) << "yllcorner" << file->YLLCORNER << std::endl
        << std::setw(HEADER_WIDTH) << "cellsize" << file->CELLSIZE << std::endl
        << std::setw(HEADER_WIDTH) << "NODATA_value" << file->NODATA_VALUE << std::endl;

    // Write the raster data
    // TODO Write the write code

    // Clean-up
    out.close();
}