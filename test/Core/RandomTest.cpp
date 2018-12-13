//
// Created by Nguyen Tran on 1/25/2018.
//

#include "Helpers/NumberHelpers.h"
#include "Core/Random.h"
#include <iostream>
#include <catch2/catch.hpp>

TEST_CASE("RandomTest", "[Core]") {

  SECTION("Good Seed Should have different value") {
    auto v1 = NumberHelpers::good_seed(1, 2, 3);
    auto v2 = NumberHelpers::good_seed(1, 2, 4);
    REQUIRE(v1!=v2);
  }

  SECTION("Shuffle for std::vector") {
    std::cout << "Shuffle Test" << std::endl;
    std::vector<int *> v_int_ptr;

    const auto number_of_individuals = 100;
    v_int_ptr.reserve(number_of_individuals);
    for (auto i = 0; i < number_of_individuals; i++) {
      v_int_ptr.push_back(new int{i});
    }

    for (auto int_ptr : v_int_ptr) {
      std::cout << *int_ptr << " ";
    }
    std::cout << std::endl;

    Random r;
    r.initialize(0);
    r.shuffle(&v_int_ptr[0], number_of_individuals, sizeof(size_t));

    for (auto int_ptr : v_int_ptr) {
      std::cout << *int_ptr << " ";
    }
    std::cout << std::endl;

    REQUIRE(*v_int_ptr[1]!=*v_int_ptr[0] + 1);

    for (auto int_ptr : v_int_ptr) {
      delete int_ptr;
    }
    v_int_ptr.clear();
  }

}
