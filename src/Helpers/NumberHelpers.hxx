/*
 * NumberHelpers.hxx
 *
 * This file defines a utility class that defines various templated functions for use with numeric data.
 *
 * NOTE that for the most part the differences between the various floating point comparisons is in their ability to be
 * self documenting; however, when checking if a floating point value is zero, then the is_zero function will be
 * nominally faster since it is able to save a couple instructions once complied.
 */
#ifndef NUMBER_HELPERS_HXX
#define NUMBER_HELPERS_HXX

class NumberHelpers {
 public:
    // Check to see if the two floating point values are equal.
    template<typename T>
    static bool is_equal(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
      return std::fabs(a - b) < epsilon;
    }

    // Check to see if the two floating point values are not equal.
    template<typename T>
    static bool is_not_equal(T a, T b, const T epsilon = std::numeric_limits<T>::epsilon()) {
      return std::fabs(a - b) >= epsilon;
    }

    // Check to see if the floating point value is zero.
    template<typename T>
    static bool is_zero(T a, const T epsilon = std::numeric_limits<T>::epsilon()) {
      return std::fabs(a) < epsilon;
    }

    // Check to see if the floating point value is not zero.
    template<typename T>
    static bool is_not_zero(T a, const T epsilon = std::numeric_limits<T>::epsilon()) {
      return std::fabs(a) >= epsilon;
    }
};

#endif
