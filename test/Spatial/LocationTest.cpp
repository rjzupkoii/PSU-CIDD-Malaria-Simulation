//
// Created by Nguyen Tran on 1/25/2018.
//
#include "gtest/gtest.h"
#include "Spatial/Location.h"

using namespace Spatial;

class LocationTest : public ::testing::Test {
protected:
    virtual void SetUp() {

    }

    virtual void TearDown() {}

};


TEST_F(LocationTest, Constructor) {
    Location loc(1, 0.123, 3.456, 10000);
    EXPECT_EQ(1, loc.id);
    EXPECT_FLOAT_EQ(0, loc.beta);
    EXPECT_FLOAT_EQ(0, loc.p_treatment);
    EXPECT_EQ(10000, loc.populationSize);
    EXPECT_FLOAT_EQ(0.123, loc.coordinate->latitude);
    EXPECT_FLOAT_EQ(3.456, loc.coordinate->longitude);
}