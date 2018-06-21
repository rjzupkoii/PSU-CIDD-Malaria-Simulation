//
// Created by Nguyen Tran on 6/21/18.
//

#ifndef PCMS_CONSTANTS_H
#define PCMS_CONSTANTS_H

#include <chrono>

class Constants {
public:
  static std::chrono::hours const &aDay() {
    static std::chrono::hours aDay_{24};
    return aDay_;
  }
};

#endif //PCMS_CONSTANTS_H
