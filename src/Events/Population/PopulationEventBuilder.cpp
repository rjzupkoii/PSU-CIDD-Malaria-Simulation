#include "PopulationEventBuilder.h"
#include <vector>
#include "yaml-cpp/yaml.h"
#include "Core/Config/Config.h"
#include "ImportationEvent.h"
#include "ImportationPeriodicallyEvent.h"

std::vector<Event*> PopulationEventBuilder::build_introduce_parasite_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (auto i = 0; i < node.size(); i++) {
    auto location = node[i]["location"].as<int>();
    if (location < config->number_of_locations()) {
      for (auto j = 0; j < node[i]["parasite_info"].size(); j++) {
        auto genotype_id = node[i]["parasite_info"][j]["genotype_id"].as<int>();
        auto time = node[i]["parasite_info"][j]["time"].as<int>();
        auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();

        auto* event = new ImportationEvent(location, time, genotype_id, num);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_parasites_periodically_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;

  for (auto i = 0; i < node.size(); i++) {
    const auto location = node[i]["location"].as<int>();
    const auto location_from = location == -1 ? 0 : location;
    const auto location_to = location == -1 ? config->number_of_locations() : min(location + 1,config->number_of_locations());

    for (auto loc = location_from; loc < location_to; ++loc) {
      for (auto j = 0; j < node[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        const auto genotype_id = node[i]["parasite_info"][j]["genotype_id"].as<int>();
        const auto dur = node[i]["parasite_info"][j]["duration"].as<int>();
        const auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();
        const auto start_day = node[i]["parasite_info"][j]["start_day"].as<int>();

        auto* event = new ImportationPeriodicallyEvent(loc, dur, genotype_id, num, start_day);
        events.push_back(event);
      }
    }
  }


  return events;
}

std::vector<Event*> PopulationEventBuilder::Build(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  const auto name = node["name"].as<std::string>();

  if (name == "introduce_parasites") {
    events = build_introduce_parasite_events(node["info"], config);
  }
  if (name == "introduce_parasites_periodically") {
    events = build_introduce_parasites_periodically_events(node["info"], config);
  }


  return events;
}
