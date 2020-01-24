/*
 * MarshallSM.hxx
 * 
 * Gravity model for human migration based upon a distance kernel function.
 * 
 * Marshall et al., 2018
 */
#ifndef MARSHALLSM_HXX
#define MARSHALLSM_HXX

#include "Helpers/NumberHelpers.h"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class MarshallSM : public SpatialModel {
        DISALLOW_COPY_AND_ASSIGN(MarshallSM)

        VIRTUAL_PROPERTY_REF(double, tau)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, rho)

        public:
            MarshallSM(const YAML::Node &node) { 
                tau_ = node["tau"].as<double>();
                alpha_ = node["alpha"].as<double>();
                rho_ = std::pow(10, node["log_rho"].as<double>());
            }

            virtual ~MarshallSM() { }

            DoubleVector get_v_relative_out_movement_to_destination(
                    const int &from_location, const int &number_of_locations,
                    const DoubleVector &relative_distance_vector,
                    const IntVector &v_number_of_residents_by_location) const override { 

                // Note the population size
                auto population = v_number_of_residents_by_location[from_location];

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

                    results[destination] = probability;
                }

                // Done, return the results
                return results;
            }    
    };
}

#endif