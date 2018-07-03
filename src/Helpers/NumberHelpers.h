//
// Created by Nguyen Tran on 6/22/18.
//

#ifndef PCMS_NUMBERHELPERS_H
#define PCMS_NUMBERHELPERS_H

#include <cmath>

class NumberHelpers {
public:
  template <typename T>
  static bool is_equal(T a, T b, double epsilon = 1e-5) {
    return std::fabs(a - b) < epsilon;
  }

  template <typename T>
  static bool is_enot_qual(T a, T b, double epsilon = 1e-5) {
    return !is_equal<T>(a, b, epsilon);
  }

  /**
   * \brief This function is used to generate good seed http://burtleburtle.net/bob/hash/doobs.html
   * \param a 
   * \param b 
   * \param c 
   * \return 
   */
  static unsigned long good_seed(unsigned long a, unsigned long b, unsigned long c) {
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

};


#endif //PCMS_NUMBERHELPERS_H
