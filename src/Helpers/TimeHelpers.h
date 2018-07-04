//
// Created by Nguyen Tran on 6/21/18.
//

#ifndef PCMS_TIMEHELPERS_H
#define PCMS_TIMEHELPERS_H

#include <sstream>
#include "date/date.h"

class TimeHelpers {
public:

  template<typename T>
  static T convert_to(const std::string& input);
};

template <typename T>
T TimeHelpers::convert_to(const std::string& input) {
	T result{};

	std::stringstream ss(input);
	date::from_stream(ss, "%Y/%m/%d", result);

	return result;
}



#endif //PCMS_TIMEHELPERS_H
