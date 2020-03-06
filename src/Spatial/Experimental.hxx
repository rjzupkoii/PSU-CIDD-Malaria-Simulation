/*
 * Experimental.hxx
 * 
 * Experimental movement equation, based upon Marshall et al. (2018) with a 
 * penalty applied based upon the travel time to the nearest city. The 
 * 
 * Marshall et al., 2018
 */
#ifndef EXPERIMENTALSM_HXX
#define EXPERIMENTALSM_HXX

#include "easylogging++.h"
#include "GIS/AscFile.h"
#include "GIS/SpatialData.h"
#include "Helpers/NumberHelpers.h"
#include "SpatialModel.hxx"
#include "yaml-cpp/yaml.h"

namespace Spatial {
    class ExperimentalSM : public SpatialModel {
        DISALLOW_COPY_AND_ASSIGN(ExperimentalSM)

        VIRTUAL_PROPERTY_REF(double, tau)
        VIRTUAL_PROPERTY_REF(double, alpha)
        VIRTUAL_PROPERTY_REF(double, rho)
        VIRTUAL_PROPERTY_REF(double, scalar)
        

        private:
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

        public:
            ExperimentalSM(const YAML::Node &node) { 
                tau_ = node["tau"].as<double>();
                alpha_ = node["alpha"].as<double>();
                rho_ = std::pow(10, node["log_rho"].as<double>());
                scalar_ = node["scalar"].as<double>();
            }

            virtual ~ExperimentalSM() { }

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