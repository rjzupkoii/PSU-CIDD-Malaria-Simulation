
#include "gtest/gtest.h"

#include "yaml-cpp/yaml.h"


TEST(yaml_cpp_test, yaml_cpp_test) {
    YAML::Node node = YAML::Load("[1, 2, 3]");
    assert(node.Type() == YAML::NodeType::Sequence);
    assert(node.IsSequence());
}
