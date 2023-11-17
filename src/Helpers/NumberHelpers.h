//
// Created by Nguyen Tran on 6/22/18.
//

#ifndef PCMS_NUMBERHELPERS_H
#define PCMS_NUMBERHELPERS_H

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <fstream>

class NumberHelpers {
 public:
  template<typename T>
  static bool is_equal(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
    return std::fabs(a - b) < epsilon;
  }

  template<typename T>
  static bool is_enot_qual(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
    return !is_equal<T>(a, b, epsilon);
  }
};

#endif
