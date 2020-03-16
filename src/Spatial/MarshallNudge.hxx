/*
 * MarshallNudge.hxx
 * 
 * Experimental movement equation, based upon Marshall et al. (2018) with a 
 * penalty applied based upon the travel time to the nearest city. The 
 * 
 * Marshall et al., 2018
 */
#ifndef MARSHALLNUDGESM_HXX
#define MARSHALLNUDGESM_HXX

#include "easylogging++.h"
#include "Helpers/NumberHelpers.h"
#include "NudgeBase.hxx"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class MarshallNudgeSM : public SpatialModel, public NudgeBase {
        DISALLOW_COPY_AND_ASSIGN(MarshallNudgeSM)

        VIRTUAL_PROPERTY_REF(double, tau)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, rho)
        VIRTUAL_PROPERTY_REF(double, scalar)
        
        public:
            MarshallNudgeSM(const YAML::Node &node) { 
                tau_ = node["tau"].as<double>();
                alpha_ = node["alpha"].as<double>();
                rho_ = std::pow(10, node["log_rho"].as<double>());
                scalar_ = node["scalar"].as<double>();
            }

            virtual ~MarshallNudgeSM() { }

            DoubleVector get_v_relative_out_movement_to_destination(
                    const int &from_location, const int &number_of_locations,
                    const DoubleVector &relative_distance_vector,
                    const IntVector &v_number_of_residents_by_location) const override { 

                // Note the population size
                auto population = v_number_of_residents_by_location[from_location];

                // Generate the normalized travel time
                double* normalized = normalize_travel(from_location, number_of_locations);

                // Prepare the vector for results
                std::vector<double> results(number_of_locations, 0.0);

                for (auto destination = 0; destination < number_of_locations; destination++) {
                    // Continue if there is nothing to do
                    if (NumberHelpers::is_equal(relative_distance_vector[destination], 0.0)) { continue; }

                    // Calculate the distance kernel
                    double distance = relative_distance_vector[destination];
                    double kernel = std::pow(1 + (distance / rho_), (-alpha_));

                    // Calculate the proportional probability
                    double probability = std::pow(population, tau_) * kernel;

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