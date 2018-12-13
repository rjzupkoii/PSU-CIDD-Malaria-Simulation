//
// Created by Nguyen Tran on 1/25/2018.
//

#include "Spatial/Location.h"
#include <catch2/catch.hpp>

using namespace Spatial;

TEST_CASE("location test", "[spatial]") {
  Location loc(1, 0.123, 3.456, 10000);

  REQUIRE(1==loc.id);
  REQUIRE(0==loc.beta);
  REQUIRE(10000==loc.population_size);
  REQUIRE(Approx(0.123)==loc.coordinate->latitude);
  REQUIRE(Approx(3.456)==loc.coordinate->longitude);
}