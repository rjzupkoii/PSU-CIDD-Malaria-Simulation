/*
 * NudgeBase.hxx
 * 
 * Experimental movement operation that allows a penalty applied based upon the travel time to the nearest city.
 * 
 */
#ifndef NUDGEBASE_HXX
#define NUDGEBASE_HXX

#include <fmt/format.h>

#include "GIS/AscFile.h"
#include "GIS/SpatialData.h"

namespace Spatial {
    class NudgeBase {
        public:
            // Generate a vector of all of the normalized travel times from the given location
            //
            // NOTE This isn't the best way to do things since we end up regenerating it 
            //      for each cell, but since the movement probablities are only generated
            //      once, the overall impact should be small.
            double* normalize_travel(const int &source, const int &locations) const {

                // Get the travel times raster
                AscFile* raster = SpatialData::get_instance().get_raster(SpatialData::SpatialFileType::Travel);
                if (raster == nullptr) {
                    throw std::runtime_error(fmt::format("{} called without travel data loaded", __FUNCTION__));
                }

                // Find the min and max values for the raster
                float min = std::numeric_limits<float>::max();
                float max = std::numeric_limits<float>::min();
                for (auto row = 0; row < raster->NROWS; row++) {
                    for (auto col = 0; col < raster->NCOLS; col++) {
                        if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }
                        min = std::min(min, raster->data[row][col]);
                        max = std::max(max, raster->data[row][col]);
                    }
                }

                // Use the min and max to normalize the raster into an array
                auto id = 0;
                double* normalized = new double[locations];
                for (auto row = 0; row < raster->NROWS; row++) {
                    for (auto col = 0; col < raster->NCOLS; col++) {
                        if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }
                        normalized[id] = (raster->data[row][col]- min) / (max - min);
                        id++;
                    }
                }

                // Return the results
                return normalized;
            }
    };
}

#endif