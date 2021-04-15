/*
 * Define the abstract class that is used for custom configuration items.
 *
 * NOTE that despite being named using the I prefix for interface notation, the 
 *      read_node function is exposed which makes this more of an abstract class.
 */
#ifndef ICONFIGITEM_H
#define ICONFIGITEM_H

#include <string>
#include <yaml-cpp/yaml.h>

#include "fmt/format.h"

class Config;

class IConfigItem {
 protected:
  Config *config_{nullptr};
  std::string name_;
 public:
  explicit IConfigItem(Config *config, const std::string &name);

  virtual ~IConfigItem() = default;

  virtual const std::string &name() {
    return name_;
  }

  virtual void set_value(const YAML::Node &node) = 0;

  // Verify the existence of the node given key before reading the value.
  template<typename T>
  inline T read_node(const YAML::Node &node, const std::string name) {
    if (!node[name]) {
      throw std::invalid_argument(fmt::format("The node with the name '{}' cannot be found.", name));
    }
    return node[name].as<T>();
  }
};

#endif
