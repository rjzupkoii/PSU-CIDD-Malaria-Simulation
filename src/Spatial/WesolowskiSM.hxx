/*
 * WesolowskiSM.hxx
 *
 * Movement model based upon gravity model in https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1004267
 */
#ifndef SPATIAL_WESOLOWSKISM_H
#define SPATIAL_WESOLOWSKISM_H

#include <cmath>

#include "Core/PropertyMacro.h"
#include "Helpers/NumberHelpers.hxx"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class WesolowskiSM : public SpatialModel {
        DISALLOW_COPY_AND_ASSIGN(WesolowskiSM)

        VIRTUAL_PROPERTY_REF(double, kappa)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, beta)
        VIRTUAL_PROPERTY_REF(double, gamma)

        public:
        explicit WesolowskiSM(const YAML::Node &node) {
            kappa_ = node["kappa"].as<double>();
            alpha_ = node["alpha"].as<double>();
            beta_ = node["beta"].as<double>();
            gamma_ = node["gamma"].as<double>();
        };

        ~WesolowskiSM() override = default;

        [[nodiscard]]
        DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                                const DoubleVector &relative_distance_vector,
                                                                const IntVector &v_number_of_residents_by_location) const override {

            std::vector<double> v_relative_number_of_circulation_by_location(number_of_locations, 0);
            for (int target_location = 0; target_location < number_of_locations; target_location++) {
                if (NumberHelpers::is_equal(relative_distance_vector[target_location], 0.0)) {
                    v_relative_number_of_circulation_by_location[target_location] = 0;
                } else {
                  // Gravity model: N_{ij}=\frac{pop^\alpha_ipop^\beta_j}{d(i,j)^\gamma}\kappa
                  v_relative_number_of_circulation_by_location[target_location] = kappa_*
                        (pow(v_number_of_residents_by_location[from_location], alpha_) *  pow(v_number_of_residents_by_location[target_location], beta_)) /
                        (pow(relative_distance_vector[target_location], gamma_));
                }
            }
            return v_relative_number_of_circulation_by_location;
        }
    };
}

#endif
