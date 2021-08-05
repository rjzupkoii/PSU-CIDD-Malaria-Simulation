/*
 * TimeHelpers.h
 *
 * Defines and implements the static TimeHelpers class which contains some useful functions for working with dates
 */
#ifndef TIMEHELPERS_H
#define TIMEHELPERS_H

#include <sstream>
#include "date/date.h"

inline std::ostream &operator<<(std::ostream &stream, const date::sys_days &o_days) {
  stream << date::year_month_day{o_days};
  return stream;
}

class TimeHelpers {
  public:
    static int number_of_days(const date::sys_days &first, const date::sys_days &last);

    static int number_of_days_to_next_year(const date::sys_days &today);

    static int get_simulation_time_birthday(const int &days_to_next_birthday, const int &age, const date::sys_days &starting_day);

    static int day_of_year(const date::sys_days &day);

    // Return true if the given year is a leap year, false otherwise.
    static bool is_leap_year(const int year) {
      return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }

    static unsigned int days_in_month(const int year, const int month) {
      if (month == 4 || month == 6 || month == 9 || month == 11) {
        // Thirty days has September,
        // April, June, and November,
        return 30;
      } else if (month != 2) {
        // All the rest have thirty-one,
        return 31;
      }

      // But February's twenty-eight,
      if (!is_leap_year(year)) {
        return 28;
      }

      // The leap year, which comes once in four,
      // Gives February one day more.
      return 29;
    }
};

inline int TimeHelpers::number_of_days(const date::sys_days &first, const date::sys_days &last) {
  return (last - first).count();
}

inline int TimeHelpers::number_of_days_to_next_year(const date::sys_days &today) {

  const date::sys_days next_year{date::year_month_day{today} + date::years{1}};
  return number_of_days(today, next_year);
}

inline int TimeHelpers::get_simulation_time_birthday(const int &days_to_next_birthday, const int &age,
                                                     const date::sys_days &starting_day) {
  const auto calendar_birthday = date::floor<date::days>(
      starting_day + date::days{days_to_next_birthday + 1} - date::years{age + 1});

  return number_of_days(starting_day, calendar_birthday);

}

inline int TimeHelpers::day_of_year(const date::sys_days &day) {
  date::year_month_day ymd{day};
  return number_of_days(date::sys_days{ymd.year()/1/0}, day);
}

#endif
