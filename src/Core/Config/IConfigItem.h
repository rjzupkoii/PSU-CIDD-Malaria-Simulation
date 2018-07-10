#ifndef ICONFIGITEM_H
#define ICONFIGITEM_H
#include <string>

namespace YAML {
  class Node;
}

class IConfigItem {
public:
  virtual const std::string& name() = 0;
  virtual void set_value(const YAML::Node& node) = 0;
};
#endif // ICONFIGITEM_H
