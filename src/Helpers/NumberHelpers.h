//
// Created by Nguyen Tran on 6/22/18.
//

#ifndef PCMS_NUMBERHELPERS_H
#define PCMS_NUMBERHELPERS_H

#include <cmath>

class NumberHelpers {
public:
  template<typename T>
  static bool is_equal(T a, T b, double epsilon = 1e-5) {
    return std::fabs(a - b) < epsilon;
  }

  template<typename T>
  static bool is_enot_qual(T a, T b, double epsilon = 1e-5) {
    return !is_equal<T>(a, b, epsilon);
  }
};


#endif //PCMS_NUMBERHELPERS_H
