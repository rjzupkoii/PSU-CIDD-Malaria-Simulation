/*
 * WesolowskiNudgeSM.hxx
 * 
 * Experimental movement equation, based upon Wesolowski equations.
 */
#ifndef WESOLOWSKINUDGE_HXX
#define WESOLOWSKINUDGE_HXX

#include "easylogging++.h"
#include "Helpers/NumberHelpers.h"
#include "NudgeBase.hxx"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class WesolowskiNudgeSM : public SpatialModel, public NudgeBase {
        DISALLOW_COPY_AND_ASSIGN(WesolowskiNudgeSM)

        VIRTUAL_PROPERTY_REF(double, kappa)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, beta)
        VIRTUAL_PROPERTY_REF(double, gamma)
        VIRTUAL_PROPERTY_REF(double, scalar)

        public:
            WesolowskiNudgeSM(const YAML::Node &node) {
                kappa_ = node["kappa"].as<double>();
                alpha_ = node["alpha"].as<double>();
                beta_ = node["beta"].as<double>();
                gamma_ = node["gamma"].as<double>();
                scalar_ = node["scalar"].as<double>();
            }

            virtual ~WesolowskiNudgeSM() = default;

            DoubleVector get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                                  const DoubleVector &relative_distance_vector,
                                                                  const IntVector &v_number_of_residents_by_location) const {

                // Note the population size
                auto population = v_number_of_residents_by_location[from_location];

                // Generate the normalized travel time
                double* normalized = normalize_travel(from_location, number_of_locations);

                // Prepare the vector for results
                std::vector<double> results(number_of_locations, 0.0);

                for (int destination = 0; destination < number_of_locations; destination++) {
                    // Continue if there is nothing to do
                    if (NumberHelpers::is_equal(relative_distance_vector[destination], 0.0)) { continue; } 

                    // Calculate the proportional probability
                    double probability = kappa_ * ((pow(population, alpha_) * pow(population, beta_)) / pow(relative_distance_vector[destination], gamma_));

                    // Apply the adjustment
                    probability = std::min(probability, probability * (scalar_ - normalized[destination]));
                    results[destination] = probability;
                }

                // Free the memory used for the normalized
                delete normalized;

                // Done, return the results
                return results;
            }
    };
}

#endif