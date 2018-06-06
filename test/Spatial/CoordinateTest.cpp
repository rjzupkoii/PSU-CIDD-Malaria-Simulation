//
// Created by Nguyen Tran on 1/25/2018.
//
#include "gtest/gtest.h"
#include "Spatial/Coordinate.h"
#include <math.h>
using namespace Spatial;

class CoordinateTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

};

TEST_F(CoordinateTest, InitializeWithLatAndLong) {
    Coordinate c(10, 100);

    EXPECT_EQ(10, c.latitude);
    EXPECT_EQ(100, c.longitude);

    Coordinate c1;

    EXPECT_EQ(0, c1.latitude);
    EXPECT_EQ(0, c1.longitude);
}

TEST_F(CoordinateTest, CalculateDistanceInKm) {
    Coordinate c1;
    Coordinate c2(80, 70);

    EXPECT_EQ(9629, round(Coordinate::calculate_distance_in_km(c1, c2)));
}