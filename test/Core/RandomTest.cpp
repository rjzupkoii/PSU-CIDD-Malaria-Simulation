//
// Created by Nguyen Tran on 1/25/2018.
//
#include "catch/catch.hpp"
#include "Random.h"
TEST_CASE("RandomTest", "[Core]") {

  SECTION("Good Seed Should have different value") {
    auto v1 = Random::good_seed();
    auto v2 = Random::good_seed();
    REQUIRE(v1 != v2);

    // Coordinate c(10, 100);
    //

    // REQUIRE(10 == c.latitude);
    // REQUIRE(100 == c.longitude);
    //
    // Coordinate c1;
    //
    // REQUIRE(0 == c1.latitude);
    // REQUIRE(0 == c1.longitude);

  }
  SECTION("CalculateDistanceInKm") {
    // Coordinate c1;
    // Coordinate c2(80, 70);
    //
    // REQUIRE(9629 == round(Coordinate::calculate_distance_in_km(c1, c2)));
  }
}
