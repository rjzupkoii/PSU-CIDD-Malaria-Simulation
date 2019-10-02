/*
 *
 */
#include "AscFile.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

// Read the indicated file from disk, caller is responsible for checking if 
// data is integer or floating point.
AscFile* AscFileManager::read(std::string fileName) {
    // Treat the struct as POD
    AscFile results = { 0 };

    // Open and read the first six lines of the header
    std::string field, value;
    std::ifstream in(fileName);
    for (auto ndx = 0; ndx < 6; ndx++) {

        // Read the field and value, cast to upper case
        in >> field >> value;
        std::transform(field.begin(), field.end(), field.begin(), ::toupper);

        // Store the values
        if (field == "NCOLS") {
            results.NCOLS = std::stoi(value);
        } else if (field == "NROWS") {
            results.NROWS = std::stoi(value);
        } else if (field == "XLLCENTER") {
            results.XLLCENTER = std::stod(value);
        } else if (field == "YLLCENTER") {
            results.YLLCENTER = std::stod(value);
        } else if (field == "XLLCORNER") {
            results.XLLCORNER = std::stod(value);
        } else if (field == "YLLCORNER") {
            results.YLLCORNER = std::stod(value);
        } else if (field == "CELLSIZE") {
            results.CELLSIZE = std::stod(value);
        } else if (field == "NODATA_VALUE") {
            results.NODATA_VALUE = std::stod(value);
        }
    }

    // The coordinate to fix the raster should either be the lower-left, or the center
    // TODO Write check code

    // Remainder of the file is the actual raster data
    // TODO Write load code

    // Clean-up
    in.close();
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