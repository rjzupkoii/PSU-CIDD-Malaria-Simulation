//
// Created by Nguyen Tran on 1/29/2018.
//

#ifndef SPATIAL_BARABASISM_HXX
#define SPATIAL_BARABASISM_HXX

#include <cmath>

#include "Core/PropertyMacro.h"
#include "Helpers/NumberHelpers.h"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class BarabasiSM : public SpatialModel {
        DISALLOW_COPY_AND_ASSIGN(BarabasiSM)

        VIRTUAL_PROPERTY_REF(double, r_g_0)
        VIRTUAL_PROPERTY_REF(double, beta_r)
        VIRTUAL_PROPERTY_REF(double, kappa)

    public:
        explicit BarabasiSM(const YAML::Node &node) {
          r_g_0_ = node["r_g_0"].as<double>();
          beta_r_ = node["beta_r"].as<double>();
          kappa_ = node["kappa"].as<double>();
        };

        ~BarabasiSM() override = default;

        [[nodiscard]]
        DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                                const DoubleVector &relative_distance_vector,
                                                                const IntVector &v_number_of_residents_by_location) const override {

          DoubleVector v_relative_number_of_circulation_by_location(number_of_locations, 0);
          for (int target_location = 0; target_location < number_of_locations; target_location++) {
            if (NumberHelpers::is_equal(relative_distance_vector[target_location], 0.0)) {
              v_relative_number_of_circulation_by_location[target_location] = 0;
            } else {
              v_relative_number_of_circulation_by_location[target_location] =
                      pow((relative_distance_vector[target_location] + r_g_0_), -beta_r_) *
                      exp(-r_g_0_ / kappa_);   // equation from Barabasi's paper
            }
          }
          return v_relative_number_of_circulation_by_location;
        }
    };
}

#endif