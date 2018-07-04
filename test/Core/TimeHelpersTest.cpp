//
// Created by Nguyen Tran on 7/3/2018.
//
#include "catch/catch.hpp"
#include "Helpers/TimeHelpers.h"
#include "date/date.h"

TEST_CASE("TimeHelpers", "[Core]") {

	SECTION("Can convert to ymd from string") {
    const std::string input = "2018/7/3";
    auto date = TimeHelpers::convert_to<date::year_month_day>(input);
		using namespace date::literals;
		REQUIRE(date == 2018_y / 7 / 3);
	}
}
