
#include "Helpers/TimeHelpers.h"
#include "date/date.h"
#include "Helpers/StringHelpers.h"
#include <catch2/catch.hpp>

using namespace date::literals;

TEST_CASE("StringHelpers", "[Core]") {

  SECTION("Can split string") {
    const std::string input = "a b c d e";
    const auto result = StringHelpers::split(input, ' ');
    REQUIRE(5==result.size());

    std::vector<std::string> expected{"a", "b", "c", "d", "e"};
    for (auto i = 0; i < 5; ++i) {
      REQUIRE(result[i]==expected[i]);
    }
  }

  SECTION("Can split string with spaces") {
    const std::string input = "a  b   c    d   e";
    const auto result = StringHelpers::split(input, ' ');
    REQUIRE(5==result.size());

    std::vector<std::string> expected{"a", "b", "c", "d", "e"};
    for (auto i = 0; i < 5; ++i) {
      REQUIRE(result[i]==expected[i]);
    }
  }
}
