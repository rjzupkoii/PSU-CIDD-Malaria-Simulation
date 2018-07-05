//
// Created by Nguyen Tran on 6/21/18.
//

#ifndef PCMS_TIMEHELPERS_H
#define PCMS_TIMEHELPERS_H

#include <sstream>
#include <iostream>
#include "date/date.h"

class TimeHelpers {
public:
  static int number_of_days(
    const int& y1, const unsigned int& m1, const unsigned int& d1,
    const int& y2, const unsigned int& m2, const unsigned int& d2);

  static int number_of_days(const date::sys_days& first, const date::sys_days& last);

  template <typename T>
  static T convert_to(const std::string& input);

  static int number_of_days_to_next_year(const date::sys_days& today);
  static int get_simulation_time_birthday(const int& days_to_next_birthday, const int& age, const date::sys_days&
                                          starting_day);
  // static int get_next_birthday(const date::sys_days& starting_day, const int& simulation_time_birthday);
};

inline int TimeHelpers::number_of_days(const int& y1, const unsigned int& m1, const unsigned int& d1, const int& y2,
                                       const unsigned int& m2, const unsigned int& d2) {
  using namespace date;
  return (sys_days{year{y2} / month{m2} / day{d2}} - sys_days{year{y1} / month{m1} / day{d1}}).count();
}

inline int TimeHelpers::number_of_days(const date::sys_days& first, const date::sys_days& last) {
  return (last - first).count();
}

template <typename T>
T TimeHelpers::convert_to(const std::string& input) {
  T result{};
  std::stringstream ss(input);
  date::from_stream(ss, "%Y/%m/%d", result);
  return result;
}

inline int TimeHelpers::number_of_days_to_next_year(const date::sys_days& today) {

  const date::sys_days next_year{date::year_month_day{today} + date::years{1}};
  return number_of_days(today, next_year);
}

inline int TimeHelpers::get_simulation_time_birthday(const int& days_to_next_birthday, const int& age,
                                                     const date::sys_days& starting_day) {
  date::year_month_day ymd{starting_day};
  const auto calendar_birthday = floor<date::days>(
    starting_day + date::days{days_to_next_birthday + 1} - date::years{age + 1});

  return number_of_days(starting_day, calendar_birthday);

}


#endif //PCMS_TIMEHELPERS_H
