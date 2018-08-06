#define NOMINMAX

#include "PopulationEventBuilder.h"
#include <vector>
#include "yaml-cpp/yaml.h"
#include "Core/Config/Config.h"
#include "ImportationEvent.h"
#include "ImportationPeriodicallyEvent.h"
#include "ChangeTreatmentCoverageEvent.h"
#include "ChangeStrategyEvent.h"
#include <algorithm>
#include "SingleRoundMDAEvent.h"

std::vector<Event*> PopulationEventBuilder::build_introduce_parasite_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    auto location = node[i]["location"].as<int>();
    if (location < config->number_of_locations()) {
      for (std::size_t j = 0; j < node[i]["parasite_info"].size(); j++) {
        auto genotype_id = node[i]["parasite_info"][j]["genotype_id"].as<int>();
        auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = node[i]["parasite_info"][j]["day"].as<date::year_month_day>();
        auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

        auto* event = new ImportationEvent(location, time, genotype_id, num);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_parasites_periodically_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;

  for (std::size_t i = 0; i < node.size(); i++) {
    const auto location = node[i]["location"].as<int>();
    const auto location_from = location == -1 ? 0 : location;
    const auto location_to = location == -1 ? config->number_of_locations() : std::min(location + 1,config->number_of_locations());

    for (auto loc = location_from; loc < location_to; ++loc) {
      for (std::size_t j = 0; j < node[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        const auto genotype_id = node[i]["parasite_info"][j]["genotype_id"].as<int>();
        const auto dur = node[i]["parasite_info"][j]["duration"].as<int>();
        const auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = node[i]["parasite_info"][j]["start_day"].as<date::year_month_day>();
        auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

        auto* event = new ImportationPeriodicallyEvent(loc, dur, genotype_id, num, time);
        events.push_back(event);
      }
    }
  }
  return events;
}


std::vector<Event*> PopulationEventBuilder::build_change_treatment_coverage_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    auto* tcm = ITreatmentCoverageModel::build(node[i], config);
    // std::cout << tcm->starting_time << std::endl;
    auto* e = new ChangeTreatmentCoverageEvent(tcm);
    events.push_back(e);
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_change_treatment_strategy_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {    
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
    auto strategy_id = node[i]["strategy_id"].as<int>();
    
    auto* e = new ChangeStrategyEvent(time, strategy_id);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_single_round_mda_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{ starting_date } -date::sys_days{ config->starting_date() }).count();
    auto* e = new SingleRoundMDAEvent(time);
    for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
      auto input_loc = node[i]["fraction_population_targeted"].size() < config->number_of_locations() ? 0 : loc;
      e->fraction_population_targeted.push_back(node[i]["fraction_population_targeted"][input_loc].as<double>());
    }
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  const auto name = node["name"].as<std::string>();

  if (name == "introduce_parasites") {
    events = build_introduce_parasite_events(node["info"], config);
  }
  if (name == "introduce_parasites_periodically") {
    events = build_introduce_parasites_periodically_events(node["info"], config);
  }
  if (name == "change_treatment_coverage") {
    events = build_change_treatment_coverage_event(node["info"], config);
  }

  if (name == "change_treatment_strategy") {
    events = build_change_treatment_strategy_event(node["info"], config);
  }

  if (name == "single_round_MDA") {
    events = build_single_round_mda_event(node["info"], config);
  }
  return events;
}
