/*
 * BurkinaFaso.hxx
 * 
 * Tuned movement model for Burkina Faso based upon Marshall et al. (2018), 
 * with a penalty applied based upon the travel time to the nearest city. 
 * Intradistrict movement in capital is also penalized as well.
 * 
 * Marshall et al., 2018
 */
#ifndef BURKINAFASO_HXX
#define BURKINAFASO_HXX

#include "easylogging++.h"
#include "Helpers/NumberHelpers.h"
#include "NudgeBase.hxx"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class BurkinaFaso : public SpatialModel, public NudgeBase {
        DISALLOW_COPY_AND_ASSIGN(BurkinaFaso)

        VIRTUAL_PROPERTY_REF(double, tau)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, rho)
        VIRTUAL_PROPERTY_REF(double, capital)
        VIRTUAL_PROPERTY_REF(double, penalty)

        const double CAPITAL_DISTRICT = 14;

        
        public:
            BurkinaFaso(const YAML::Node &node) { 
                tau_ = node["tau"].as<double>();
                alpha_ = node["alpha"].as<double>();
                rho_ = std::pow(10, node["log_rho"].as<double>());
                capital_ = node["capital"].as<double>();
                penalty_ = node["penalty"].as<double>();
            }

            virtual ~BurkinaFaso() { }

            DoubleVector get_v_relative_out_movement_to_destination(
                    const int &from_location, const int &number_of_locations,
                    const DoubleVector &relative_distance_vector,
                    const IntVector &v_number_of_residents_by_location) const override { 

                // NOTE this variable is STATIC
                static double* travel = nullptr;

                // Note the population size
                auto population = v_number_of_residents_by_location[from_location];

                // Note the source district
                auto source_district = SpatialData::get_instance().get_district(from_location);

                // Get the relevent surfaces, preparing the surface is an expensive operation,
                // so only do it once since the population should generally continue to grow
                // proportionately for each cell
                if (travel == nullptr) {
                    travel = prepare_surface(SpatialData::SpatialFileType::Travel, number_of_locations);    
                }
                
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

                    // Adjust the probability by the friction surface
                    probability = probability / (1 + travel[from_location] + travel[destination] );

                    // If the source and the destination are both in the capital district,
                    // penalize the travel by 50%
                    if (source_district == capital_ && 
                        SpatialData::get_instance().get_district(destination) == capital_) {
                            probability /= penalty_;
                    }

                    results[destination] = probability;
                }

                // Done, return the results
                return results;
            }
    };
}

#endif