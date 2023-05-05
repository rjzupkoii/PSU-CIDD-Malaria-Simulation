/*
 * SpatialModel.hxx
 *
 * Base abstract class for the spatial movement models that are implemented in the simulation.
 */
#ifndef SPATIAL_SPATIALMODEL_H
#define SPATIAL_SPATIALMODEL_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Model.h"

namespace Spatial {
    class SpatialModel {
    DISALLOW_COPY_AND_ASSIGN(SpatialModel)


    protected:
        // Prepare the travel raster for the movement model
        static void prepare_surface(const SpatialData::SpatialFileType type, double *travel) {
          // Get the travel times raster
          AscFile *raster = SpatialData::get_instance().get_raster(type);
          if (raster == nullptr) {
            throw std::runtime_error(fmt::format("{} called without travel data loaded", __FUNCTION__));
          }

          // Use the min and max to normalize the raster into an array
          auto id = 0;
          travel = new double[Model::CONFIG->number_of_locations()];
          for (auto row = 0; row < raster->NROWS; row++) {
            for (auto col = 0; col < raster->NCOLS; col++) {
              if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }
              travel[id] = raster->data[row][col];
              id++;
            }
          }
        }

    public:
        SpatialModel() = default;

        virtual ~SpatialModel() = default;

        // Allow the spatial model to perform any preparation it must do.
        virtual void prepare() { }

        [[nodiscard]]
        virtual DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                                        const DoubleVector &relative_distance_vector,
                                                                        const IntVector &v_number_of_residents_by_location) const = 0;;

    };
}

#endif
