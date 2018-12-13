//
// Created by Nguyen Tran on 1/25/2018.
//

#include "Spatial/Coordinate.h"
#include <math.h>
#include <catch2/catch.hpp>

using namespace Spatial;

TEST_CASE("CoordinateTest", "[Spatial]") {

  SECTION("InitializeWithLatAndLong") {
    Coordinate c(10, 100);

    REQUIRE(10==c.latitude);
    REQUIRE(100==c.longitude);

    Coordinate c1;

    REQUIRE(0==c1.latitude);
    REQUIRE(0==c1.longitude);

  }
  SECTION("CalculateDistanceInKm") {
    Coordinate c1;
    Coordinate c2(80, 70);

    REQUIRE(9629==round(Coordinate::calculate_distance_in_km(c1, c2)));
  }
}
