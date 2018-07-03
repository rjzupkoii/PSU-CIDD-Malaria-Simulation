//
// Created by Nguyen Tran on 1/25/2018.
//
#include "catch/catch.hpp"
#include "Helpers/NumberHelpers.h"
TEST_CASE("RandomTest", "[Core]") {

  SECTION("Good Seed Should have different value") {
    auto v1 = NumberHelpers::good_seed(1, 2, 3);
    auto v2 = NumberHelpers::good_seed(1, 2, 4);
    REQUIRE(v1 != v2);
  }  
}
