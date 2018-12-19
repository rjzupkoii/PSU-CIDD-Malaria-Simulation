//
// Created by Nguyen Tran on 1/29/2018.
//

#include "WesolowskiSM.h"
#include "Helpers/NumberHelpers.h"
#include <cmath>

namespace Spatial {

WesolowskiSM::WesolowskiSM(const YAML::Node &node) {
  kappa_ = node["kappa"].as<double>();
  alpha_ = node["alpha"].as<double>();
  beta_ = node["beta"].as<double>();
  gamma_ = node["gamma"].as<double>();
//        std::cout << beta_ << std::endl;
}

WesolowskiSM::~WesolowskiSM() = default;

DoubleVector
WesolowskiSM::get_v_relative_out_movement_to_destination(const int &from_location, const int &number_of_locations,
                                                         const DoubleVector &relative_distance_vector,
                                                         const IntVector &v_number_of_residents_by_location) const {

  std::vector<double> v_relative_number_of_circulation_by_location(number_of_locations, 0);
  for (int target_location = 0; target_location < number_of_locations; target_location++) {
    if (NumberHelpers::is_equal(relative_distance_vector[target_location], 0.0)) {
      v_relative_number_of_circulation_by_location[target_location] = 0;
    } else {
      v_relative_number_of_circulation_by_location[target_location] =
          kappa_*
              (pow(v_number_of_residents_by_location[from_location],
                   alpha_)*
                  pow(v_number_of_residents_by_location[target_location],
                      beta_))/
              (pow(relative_distance_vector[target_location],
                   gamma_));
    }
  }

  return v_relative_number_of_circulation_by_location;

}
}