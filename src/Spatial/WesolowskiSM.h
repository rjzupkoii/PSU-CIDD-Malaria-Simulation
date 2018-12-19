//
// Created by Nguyen Tran on 1/29/2018.
//

#ifndef SPATIAL_WESOLOWSKISM_H
#define SPATIAL_WESOLOWSKISM_H

#include "Core/PropertyMacro.h"
#include "SpatialModel.h"
#include "yaml-cpp/yaml.h"

namespace Spatial {

class WesolowskiSM : public SpatialModel {
 DISALLOW_COPY_AND_ASSIGN(WesolowskiSM)

 VIRTUAL_PROPERTY_REF(double, kappa)

 VIRTUAL_PROPERTY_REF(double, alpha)

 VIRTUAL_PROPERTY_REF(double, beta)

 VIRTUAL_PROPERTY_REF(double, gamma)

 public:
  WesolowskiSM(const YAML::Node &node);

  virtual ~WesolowskiSM();

  DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                          const DoubleVector &relative_distance_vector,
                                                          const IntVector &v_number_of_residents_by_location) const override;
};
}

#endif //SPATIAL_WESOLOWSKISM_H
