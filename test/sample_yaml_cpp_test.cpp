#include "yaml-cpp/yaml.h"
#include <catch2/catch.hpp>

TEST_CASE("yaml_cpp_test", "[sampleyaml_cpp_test]") {
  YAML::Node node = YAML::Load("[1, 2, 3]");
  REQUIRE(node.Type()==YAML::NodeType::Sequence);
  REQUIRE(node.IsSequence());

}

