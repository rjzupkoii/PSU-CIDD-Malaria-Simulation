
#ifndef POPULATIONEVENTBUILDER_H
#define POPULATIONEVENTBUILDER_H
#include "Events/Event.h"
#include <vector>

class Config;

namespace YAML {
  class Node;
}

class PopulationEventBuilder {
public:
  static std::vector<Event*> build_introduce_parasite_events(const YAML::Node& node,Config* config);
  static std::vector<Event*> build_introduce_parasites_periodically_events(const YAML::Node& node, Config* config);
  static std::vector<Event*> Build(const YAML::Node& node,Config* config);
};
#endif // POPULATIONEVENTBUILDER_H
