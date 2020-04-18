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
  static std::vector<Event*> build_introduce_parasite_events(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_introduce_parasites_periodically_events(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_change_treatment_coverage_event(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_change_treatment_strategy_event(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_single_round_mda_event(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_modify_nested_mft_strategy_event(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_turn_on_mutation_event(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_turn_off_mutation_event(const YAML::Node& node, Config* config);


  static std::vector<Event*> build(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_introduce_plas2_parasite_events(const YAML::Node& node, Config* config);
  static std::vector<Event*> build_introduce_580Y_mutant_events(const YAML::Node& node, Config* config);

  static std::vector<Event*> build_introduce_aq_mutant_parasite_events(const YAML::Node& node, Config* config);

  static std::vector<Event*>
  build_introduce_lumefantrine_mutant_parasite_events(const YAML::Node& node, Config* config);

};

#endif // POPULATIONEVENTBUILDER_H
