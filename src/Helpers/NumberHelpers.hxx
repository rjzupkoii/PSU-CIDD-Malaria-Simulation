/*
 * NumberHelpers.hxx
 *
 * This file defines a utility class that defines various templated functions for use with numeric data.
 */
#ifndef NUMBER_HELPERS_HXX
#define NUMBER_HELPERS_HXX

class NumberHelpers {
 public:
  template<typename T>
  static bool is_equal(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
    return std::fabs(a - b) < epsilon;
  }

  template<typename T>
  static bool is_not_equal(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
    return !is_equal<T>(a, b, epsilon);
  }
};

#endif
