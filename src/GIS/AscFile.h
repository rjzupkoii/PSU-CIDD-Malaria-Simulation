/*
 * AscFile.h
 * 
 * Define the means of reading and writing ESRI ASCII files.
 * 
 * http://webhelp.esri.com/arcgisdesktop/9.3/index.cfm?TopicName=ESRI%20ASCII%20Raster%20format 
 */
#ifndef ASCFILE_H
#define ASCFILE_H

#include <string>
#include <variant>

// The ASC file either as read, or to be written. Note that since the specification does not
// provide a header indicating if the data is floating point or integer, the data is presumed
// to be floathing point. 
struct AscFile {
    // Number of rows
    int NROWS;

    // Number of columns
    int NCOLS;

    // X-center coordinate
    double XLLCENTER;

    // Y-center coordinate
    double YLLCENTER;

    // X-lower left corner coordinate
    double XLLCORNER;
    
    // X-lower left corner coordinate
    double YLLCORNER;

    // Size of a cell, in units of projection
    double CELLSIZE;

    // Representation of a cell with no data
    double NODATA_VALUE;

    // The data stored in the file
    double* data;
};

class AscFileManager {

    private:
        static const int HEADER_WIDTH = 14;

        // Static class, no need to instansiate.
        AscFileManager() { }

    public:
        static AscFile* read(std::string fileName);
        static void write(AscFile* file, std::string fileName, int precision);
};

#endif