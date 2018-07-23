/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StrategyBuilder.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2017, 11:03 AM
 */

#include "StrategyBuilder.h"
#include "IStrategy.h"
#include "SFTStrategy.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include "CyclingStrategy.h"
#include "AdaptiveCyclingStrategy.h"
#include "MFTStrategy.h"
#include "ACTIncreaseStrategy.h"
#include "NovelNonACTSwitchingStrategy.h"
#include "TACTSwitchingStrategy.h"
#include "NestedSwitchingStrategy.h"
#include "MFTRebalancingStrategy.h"
#include "MFTDifferentDistributionByLocationStrategy.h"
#include "NestedSwitchingDifferentDistributionByLocationStrategy.h"

StrategyBuilder::StrategyBuilder() = default;

StrategyBuilder::~StrategyBuilder() = default;

IStrategy* StrategyBuilder::build(const YAML::Node& ns, const int& strategy_id, Config* config) {
  const auto type = IStrategy::StrategyTypeMap[ns["type"].as<std::string>()];
  switch (type) {
  case IStrategy::SFT:
    return buildSFTStrategy(ns, strategy_id, config);
  case IStrategy::Cycling:
    return buildCyclingStrategy(ns, strategy_id, config);
  case IStrategy::AdaptiveCycling:
    return buildAdaptiveCyclingStrategy(ns, strategy_id, config);
  case IStrategy::MFT:
    return buildMFTStrategy(ns, strategy_id, config);
  case IStrategy::ACTIncreaseOvertime:
    return buildACTIncreaseStrategy(ns, strategy_id, config);
  case IStrategy::NovelNonACTSwitching:
    return buildNovelNonACTSwitchingStrategy(ns, strategy_id, config);
  case IStrategy::TACTSwitching:
    return buildTACTSwitchingStrategy(ns, strategy_id, config);
  case IStrategy::MFTRebalancing:
    return buildMFTRebalancingStrategy(ns, strategy_id, config);
  case IStrategy::NestedSwitching:
    return buildNestedSwitchingStrategy(ns, strategy_id, config);
  case IStrategy::MFTDifferentDistributionByLocation:
    return buildMFTDifferentDistributionByLocationStrategy(ns, strategy_id, config);
  case IStrategy::NestedSwitchingDifferentDistributionByLocation:
    return buildNestedSwitchingDifferentDistributionByLocationStrategy(ns, strategy_id, config);
  default:
    return nullptr;
  }
}

void StrategyBuilder::add_therapies(const YAML::Node& ns, IStrategy* result, Config* config) {
  for (auto i = 0; i < ns["therapy_ids"].size(); i++) {
    result->add_therapy(config->therapy_db()[ns["therapy_ids"][i].as<int>()]);
  }
}

void StrategyBuilder::add_distributions(const YAML::Node& ns, DoubleVector& v) {
  for (auto i = 0; i < ns.size(); i++) {
    v.push_back(ns[i].as<double>());
  }
}

IStrategy* StrategyBuilder::buildSFTStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new SFTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();
  result->add_therapy(config->therapy_db()[ns["therapy_id"].as<int>()]);
  return result;
}

IStrategy* StrategyBuilder::buildCyclingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new CyclingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->cycling_time = ns["cycling_time"].as<int>();
  result->next_switching_day = ns["cycling_time"].as<int>();

  add_therapies(ns, result, config);

  return result;
}

IStrategy* StrategyBuilder::buildAdaptiveCyclingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new AdaptiveCyclingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->trigger_value = ns["trigger_value"].as<double>();
  result->delay_until_actual_trigger = ns["delay_until_actual_trigger"].as<int>();
  result->turn_off_days = ns["turn_off_days"].as<int>();

  add_therapies(ns, result, config);
  return result;
}

IStrategy* StrategyBuilder::buildMFTStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new MFTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["distribution"], result->distribution);
  add_therapies(ns, result, config);
  return result;
}

IStrategy* StrategyBuilder::buildACTIncreaseStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new ACTIncreaseStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["start_distribution"], result->start_distribution);
  add_distributions(ns["start_distribution"], result->distribution);
  add_distributions(ns["end_distribution"], result->end_distribution);

  add_therapies(ns, result, config);

  return result;
}

IStrategy* StrategyBuilder::buildNovelNonACTSwitchingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new NovelNonACTSwitchingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["distribution"], result->distribution);
  add_therapies(ns, result, config);

  result->non_artemisinin_switching_day = ns["non_artemisinin_switching_day"].as<int>();
  result->non_art_therapy_id = ns["non_art_therapy_id"].as<int>();
  result->fraction_non_art_replacement = ns["fraction_non_art_replacement"].as<double>();

  return result;
}

IStrategy* StrategyBuilder::buildTACTSwitchingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new TACTSwitchingTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();


  add_distributions(ns["start_distribution"], result->start_distribution);
  add_distributions(ns["start_distribution"], result->distribution);
  add_distributions(ns["end_distribution"], result->end_distribution);

  add_therapies(ns, result, config);

  result->TACT_switching_day = ns["TACT_switching_day"].as<int>();
  result->TACT_id = ns["TACT_id"].as<int>();
  return result;
}

IStrategy* StrategyBuilder::buildNestedSwitchingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new NestedSwitchingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["start_distribution"], result->start_distribution);
  add_distributions(ns["start_distribution"], result->distribution);
  add_distributions(ns["end_distribution"], result->end_distribution);

  for (int i = 0; i < ns["strategy_ids"].size(); i++) {
    result->add_strategy(
      config->strategy_db()[ns["strategy_ids"][i].as<int>()]);
  }

  result->strategy_switching_day = ns["strategy_switching_day"].as<int>();
  result->switch_to_strategy_id = ns["switch_to_strategy_id"].as<int>();


  return result;
}

IStrategy* StrategyBuilder::buildMFTRebalancingStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new MFTRebalancingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["distribution"], result->distribution);
  add_distributions(ns["distribution"], result->next_distribution);

  add_therapies(ns, result, config);

  result->update_duration_after_rebalancing = ns["update_duration_after_rebalancing"].as<int>();
  result->delay_until_actual_trigger = ns["delay_until_actual_trigger"].as<int>();
  result->latest_adjust_distribution_time = 0;

  return result;
}

IStrategy* StrategyBuilder::buildMFTDifferentDistributionByLocationStrategy(const YAML::Node& ns, const int& strategy_id, Config* config) {
  auto* result = new MFTDifferentDistributionByLocationStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->distribution_by_location.clear();
  result->distribution_by_location.resize(
    static_cast<unsigned long long int>(config->number_of_locations()));

  for (auto loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["distribution"][input_loc], result->distribution_by_location[loc]);
  }

  add_therapies(ns, result, config);

  return result;
}

IStrategy* StrategyBuilder::buildNestedSwitchingDifferentDistributionByLocationStrategy(const YAML::Node& ns,
                                                                                        const int& strategy_id, Config* config) {
  auto* result = new NestedSwitchingDifferentDistributionByLocationStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->distribution.clear();
  result->distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  result->start_distribution.clear();
  result->start_distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  for (auto loc = 0; loc < config->number_of_locations(); loc++) {
    int input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->distribution[loc]);
  }
  for (auto loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->start_distribution[loc]);
  }

  for (auto i = 0; i < ns["strategy_ids"].size(); i++) {
    result->add_strategy(config->strategy_db()[ns["strategy_ids"][i].as<int>()]);
  }

  result->peak_at = ns["peak_at"].as<int>();
  result->strategy_switching_day = ns["strategy_switching_day"].as<int>();
  result->switch_to_strategy_id = ns["switch_to_strategy_id"].as<int>();

  //    std::cout << result->to_string() << std::endl;

  return result;
}
