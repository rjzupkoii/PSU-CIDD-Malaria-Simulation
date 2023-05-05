/*
 * WesolowskiSurfaceSM.hxx
 *
 * Movement model based upon gravity model in https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1004267
 * with the travel surface penalty from https://www.nature.com/articles/s41598-022-26878-5 applied to the results.
 */
#ifndef SPATIAL_WESOLOWSKISURFACESM_H
#define SPATIAL_WESOLOWSKISURFACESM_H

#include <cmath>

#include "Core/PropertyMacro.h"
#include "Helpers/NumberHelpers.h"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class WesolowskiSurfaceSM : public SpatialModel {
    DISALLOW_COPY_AND_ASSIGN(WesolowskiSurfaceSM)

    VIRTUAL_PROPERTY_REF(double, kappa)
    VIRTUAL_PROPERTY_REF(double, alpha)
    VIRTUAL_PROPERTY_REF(double, beta)
    VIRTUAL_PROPERTY_REF(double, gamma)

    private:
        // Travel surface, computed when the prepare method is called
        double* travel = nullptr;

    public:
        explicit WesolowskiSurfaceSM(const YAML::Node &node) {
          kappa_ = node["kappa"].as<double>();
          alpha_ = node["alpha"].as<double>();
          beta_ = node["beta"].as<double>();
          gamma_ = node["gamma"].as<double>();
        };

        ~WesolowskiSurfaceSM() override = default;

        void prepare() override {
          prepare_surface(SpatialData::Travel, travel);
        }

        [[nodiscard]]
        DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                                const DoubleVector &relative_distance_vector,
                                                                const IntVector &v_number_of_residents_by_location) const override {

          std::vector<double> results(number_of_locations, 0);
          for (int destination = 0; destination < number_of_locations; destination++) {
            if (NumberHelpers::is_equal(relative_distance_vector[destination], 0.0)) {
              results[destination] = 0;
            } else {
              // Gravity model: N_{ij}=\frac{pop^\alpha_ipop^\beta_j}{d(i,j)^\gamma}\kappa
              auto probability = kappa_ *
                      (pow(v_number_of_residents_by_location[from_location],alpha_) * pow(v_number_of_residents_by_location[destination], beta_)) /
                      (pow(relative_distance_vector[destination], gamma_));

              // Travel penalty: Pr(j|i)' = Pr(j|i) / (1 + t_i + t_j)
              results[destination] = probability / (1 + travel[from_location] + travel[destination]);
            }
          }
          return results;
        }
    };
}

#endif
