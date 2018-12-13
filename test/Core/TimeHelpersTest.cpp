//
// Created by Nguyen Tran on 7/3/2018.
//

#include "Helpers/TimeHelpers.h"
#include "date/date.h"
#include <catch2/catch.hpp>

using namespace date::literals;

TEST_CASE("TimeHelpers", "[Core]") {

  SECTION("Can convert to ymd from string") {
    const std::string input = "2018/7/3";
    auto date = TimeHelpers::convert_to<date::year_month_day>(input);
    REQUIRE(date==2018_y/7/3);
  }

  SECTION("Can get number of days between 2 sysdays") {
    auto expected = TimeHelpers::number_of_days(2018, 1, 1, 2018, 1, 2);

    REQUIRE(expected==1);
  }
  SECTION("get the number of days until next year for normal year") {
    auto expected = 0;
    // for (int i = 0; i < 10000000 * 40; ++i) {
    expected = TimeHelpers::number_of_days_to_next_year(date::sys_days{2018_y/3/4});
    // }
    REQUIRE(expected==365);
  }

  SECTION("get the number of days until next year for leap year") {
    //2000 is leap year
    auto expected = TimeHelpers::number_of_days_to_next_year(date::sys_days{1999_y/3/4});
    REQUIRE(expected==366);

    expected = TimeHelpers::number_of_days_to_next_year(date::sys_days{1999_y/2/28});
    REQUIRE(expected==365);
  }

  SECTION("get simulation time birthday ") {
    using namespace date::literals;

    // today, starting_day: 1999/1/20
    // days_to_next_birthday: 9 => 1999/1/29
    // current_age:0
    // birthday should be 1998/1/29
    // simulation_time_bithday should be -10

    const auto days_to_next_birthday = 9; // range from [0, 365)

    const auto age = 0;
    auto today{1999_y/1/20};
    auto expected = TimeHelpers::get_simulation_time_birthday(days_to_next_birthday, age, date::sys_days{today});
    // std::cout << expected << std::endl;

    auto birthday{date::sys_days{1998_y/1/29}};
    REQUIRE(expected==TimeHelpers::number_of_days(today, birthday));
  }

  SECTION("get simulation time birthday 2 ") {
    using namespace date::literals;

    // today, starting_day: 1999/1/20
    // days_to_next_birthday: 365 => 2000/1/19
    // current_age:0
    // birthday should be 1999/1/19
    // simulation_time_bithday should be -1

    const auto days_to_next_birthday = 364; // range from [0, 365)
    const auto age = 0;
    auto today{1999_y/1/20};
    auto expected = TimeHelpers::get_simulation_time_birthday(days_to_next_birthday, age, date::sys_days{today});
    auto birthday{date::sys_days{1999_y/1/19}};
    REQUIRE(expected==TimeHelpers::number_of_days(today, birthday));
  }
  SECTION("get day of year ") {
    using namespace date::literals;
    date::sys_days d1{2018_y/1/30};
    REQUIRE(30==TimeHelpers::day_of_year(d1));

    date::sys_days d2{2018_y/6/24};
    REQUIRE(175==TimeHelpers::day_of_year(d2));

    date::sys_days d3{2000_y/6/24};
    REQUIRE(176==TimeHelpers::day_of_year(d3));

  }

}
