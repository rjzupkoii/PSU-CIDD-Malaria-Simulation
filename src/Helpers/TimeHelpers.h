//
// Created by Nguyen Tran on 6/21/18.
//

#ifndef PCMS_TIMEHELPERS_H
#define PCMS_TIMEHELPERS_H

#include <ctime>
#include <ostream>
#include <iomanip>

/// Output timepoint to stream
/// \tparam Clock
/// \tparam Duration
/// \param os
/// \param time_point
/// \return
template<typename Clock, typename Duration>
std::ostream &operator<<(std::ostream &os, const std::chrono::time_point<Clock, Duration> &time_point) {
  auto converted_timepoint = Clock::to_time_t(time_point);
  os << std::put_time(std::localtime(&converted_timepoint), "%d\t%m\t%Y");
  return os;
}

class TimeHelpers {
public:
  /// Create a chrono time point from year, month, day <br>
  /// ex: create_timepoint(2000, 1, 10) create a time point for Jan 10th 2000
  /// \@param year the input year, ex: 1998
  /// \@param month the input month [1-12]
  /// \@param day the input day in month [1-31]
  /// \@return chrono timepoint
  static std::chrono::system_clock::time_point create_time_point(int year, int month, int day) {
    //perform checks, do division, modulus and stuff...
    year -= 1900; //epoch
    std::tm date = {};
    date.tm_year = year;
    date.tm_mon = month - 1;
    date.tm_mday = day;

    return std::chrono::system_clock::from_time_t(std::mktime(&date));
  }


  /// Round down duration to To (days, hours, minutes,...)
  /// \tparam To
  /// \tparam Rep
  /// \tparam Period
  /// \param d Duration
  /// \return Duration
  template<typename To, typename Rep, typename Period>
  static To round_down(const std::chrono::duration<Rep, Period> &d) {
    To t = std::chrono::duration_cast<To>(d);
    if (t > d)
      --t;
    return t;
  }

/// Returns number of days since civil 1970-01-01.  Negative values indicate days prior to 1970-01-01.<br>
/// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
///                 Exact range of validity is:
///                 [civil_from_days(numeric_limits<Int>::min()),
///                  civil_from_days(numeric_limits<Int>::max()-719468)]
/// \tparam Int
/// \param y year is "approximately" in
/////                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
/// \param m month is in [1, 12]
/// \param d day is in [1, last_day_of_month(y, m)]
/// \return
//  template<typename Int>
//  constexpr Int days_from_civil(Int y, unsigned m, unsigned d) noexcept {
//    static_assert(std::numeric_limits<unsigned>::digits >= 18,
//                  "This algorithm has not been ported to a 16 bit unsigned integer");
//    static_assert(std::numeric_limits<Int>::digits >= 20,
//                  "This algorithm has not been ported to a 16 bit signed integer");
//    y -= m <= 2;
//    const Int era = (y >= 0 ? y : y - 399) / 400;
//    const auto yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
//    const auto doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
//    const auto doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;         // [0, 146096]
//    return era * 146097 + static_cast<Int>(doe) - 719468;
//  }

  struct ymd {
    int year;
    unsigned month;
    unsigned day;
  };

  /// Returns year/month/day triple in civil calendar <br>
  /// \tparam Int
  /// \param z is number of days since 1970-01-01 and is in the range:
  /// [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
  /// \return year/month/day triple in civil calendar
  template<typename Int>
  static constexpr ymd civil_from_days(Int z) noexcept {
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
                  "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
                  "This algorithm has not been ported to a 16 bit signed integer");
    z += 719468;
    const Int era = (z >= 0 ? z : z - 146096) / 146097;
    const auto doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const auto yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;  // [0, 399]
    const Int y = static_cast<Int>(yoe) + era * 400;
    const auto doy = doe - (365 * yoe + yoe / 4 - yoe / 100);                // [0, 365]
    const auto mp = (5 * doy + 2) / 153;                                   // [0, 11]
    const auto d = doy - (153 * mp + 2) / 5 + 1;                             // [1, 31]
    const auto m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    return ymd{y + (m <= 2), m, d};
  }

  /// Get day in month of a timepoint
  /// \tparam Clock
  /// \tparam Duration
  /// \param tp input time point
  /// \return ymd struct
  template<typename Clock, typename Duration>
  static ymd get_ymd_from_time_point(const std::chrono::time_point<Clock, Duration> &tp) {
    using namespace std;
    using namespace std::chrono;
    typedef duration<int, ratio_multiply<hours::period, ratio<24>>> days;
    // t is time duration since 1970-01-01
    Duration t = tp.time_since_epoch();
    // d is days since 1970-01-01
    days d = round_down<days>(t);
    // break d down into year/month/day
    ymd ymd = civil_from_days(d.count());

    return ymd;
  }
};

#endif //PCMS_TIMEHELPERS_H
