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

  /**
   * \brief This function is used to generate good seed http://burtleburtle.net/bob/hash/doobs.html
   * \param a
   * \param b
   * \param c
   * \return
   */
  static unsigned long good_seed(unsigned long a, unsigned long b, unsigned long c) {
    std::ifstream file("/dev/urandom", std::ios::binary);
    if (file.is_open()) {
      //for unix
      const int size = sizeof(int);
      auto* memblock = new char[size];
      file.read(memblock, size);
      file.close();
      const unsigned int random_seed_a = static_cast<const unsigned int>(*reinterpret_cast<int*>(memblock));
      delete[] memblock;
      return random_seed_a ^ b;
    }
    //for windows
    a = a - b;
    a = a - c;
    a = a ^ (c >> 13);
    b = b - c;
    b = b - a;
    b = b ^ (a << 8);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 13);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 12);
    b = b - c;
    b = b - a;
    b = b ^ (a << 16);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 5);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 3);
    b = b - c;
    b = b - a;
    b = b ^ (a << 10);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 15);
    return c;
  }

  template<typename T>
  static std::string number_to_string(T number) {
    std::ostringstream ss;
    ss << number;
    return ss.str();
  }
};

#endif //PCMS_NUMBERHELPERS_H
