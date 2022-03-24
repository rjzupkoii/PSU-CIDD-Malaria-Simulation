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
#include "CyclingStrategy.h"
#include "AdaptiveCyclingStrategy.h"
#include "MFTStrategy.h"
#include "NestedMFTStrategy.h"
#include "MFTRebalancingStrategy.h"
#include "MFTMultiLocationStrategy.h"
#include "NestedMFTMultiLocationStrategy.h"
#include "NovelDrugSwitchingStrategy.h"

StrategyBuilder::StrategyBuilder() = default;

StrategyBuilder::~StrategyBuilder() = default;

IStrategy* StrategyBuilder::build(const YAML::Node &ns, const int &strategy_id, Config* config) {
  try {
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
      case IStrategy::MFTRebalancing:
        return buildMFTRebalancingStrategy(ns, strategy_id, config);
      case IStrategy::NestedMFT:
        return buildNestedSwitchingStrategy(ns, strategy_id, config);
      case IStrategy::MFTMultiLocation:
        return buildMFTMultiLocationStrategy(ns, strategy_id, config);
      case IStrategy::NestedMFTMultiLocation:
        return buildNestedMFTDifferentDistributionByLocationStrategy(ns, strategy_id, config);
      case IStrategy::NovelDrugSwitching:
        return buildNovelDrugSwitchingStrategy(ns, strategy_id, config);
      default:
        LOG(WARNING) << "Unknown strategy type: " << ns["type"].as<std::string>();
        return nullptr;
    }
  } catch (YAML::InvalidNode &error) {
    LOG(ERROR) << "Unrecoverable error parsing strategy YAML value:\n\t" << error.msg;
    exit(1);
  }
}

void StrategyBuilder::add_therapies(const YAML::Node &ns, IStrategy* result, Config* config) {
  for (std::size_t i = 0; i < ns["therapy_ids"].size(); i++) {
    result->add_therapy(config->therapy_db()[ns["therapy_ids"][i].as<int>()]);
  }
}

void StrategyBuilder::add_distributions(const YAML::Node &ns, DoubleVector &v) {
  for (std::size_t i = 0; i < ns.size(); i++) {
    v.push_back(ns[i].as<double>());
  }
}

IStrategy* StrategyBuilder::buildSFTStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new SFTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();
  result->add_therapy(config->therapy_db()[ns["therapy_id"].as<int>()]);
  return result;
}

IStrategy* StrategyBuilder::buildCyclingStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new CyclingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->cycling_time = ns["cycling_time"].as<int>();
  result->next_switching_day = ns["cycling_time"].as<int>();

  add_therapies(ns, result, config);

  return result;
}

IStrategy* StrategyBuilder::buildAdaptiveCyclingStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new AdaptiveCyclingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->trigger_value = ns["trigger_value"].as<double>();
  result->delay_until_actual_trigger = ns["delay_until_actual_trigger"].as<int>();
  result->turn_off_days = ns["turn_off_days"].as<int>();

  add_therapies(ns, result, config);
  return result;
}

IStrategy* StrategyBuilder::buildMFTStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new MFTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["distribution"], result->distribution);
  add_therapies(ns, result, config);
  return result;
}

IStrategy* StrategyBuilder::buildNestedSwitchingStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new NestedMFTStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["start_distribution"], result->start_distribution);
  add_distributions(ns["start_distribution"], result->distribution);
  add_distributions(ns["peak_distribution"], result->peak_distribution);

  result->peak_after = ns["peak_after"].as<int>();

  for (std::size_t i = 0; i < ns["strategy_ids"].size(); i++) {
    result->add_strategy(
        config->strategy_db()[ns["strategy_ids"][i].as<int>()]);
  }

  return result;
}

IStrategy* StrategyBuilder::buildMFTRebalancingStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
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

IStrategy*
StrategyBuilder::buildMFTMultiLocationStrategy(const YAML::Node &ns, const int &strategy_id, Config* config) {
  auto* result = new MFTMultiLocationStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->distribution.clear();
  result->distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  result->start_distribution.clear();
  result->start_distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  result->peak_distribution.clear();
  result->peak_distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->distribution[loc]);
  }
  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->start_distribution[loc]);
  }

  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["peak_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["peak_distribution"][input_loc], result->peak_distribution[loc]);
  }

  add_therapies(ns, result, config);
  result->peak_after = ns["peak_after"].as<int>();
  return result;
}

IStrategy* StrategyBuilder::buildNestedMFTDifferentDistributionByLocationStrategy(const YAML::Node &ns,
                                                                                  const int &strategy_id,
                                                                                  Config* config) {
  auto* result = new NestedMFTMultiLocationStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  result->distribution.clear();
  result->distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  result->start_distribution.clear();
  result->start_distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  result->peak_distribution.clear();
  result->peak_distribution.resize(static_cast<unsigned long long int>(config->number_of_locations()));

  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->distribution[loc]);
  }
  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["start_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["start_distribution"][input_loc], result->start_distribution[loc]);
  }

  for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
    auto input_loc = ns["peak_distribution"].size() < config->number_of_locations() ? 0 : loc;
    add_distributions(ns["peak_distribution"][input_loc], result->peak_distribution[loc]);
  }

  for (std::size_t i = 0; i < ns["strategy_ids"].size(); i++) {
    result->add_strategy(config->strategy_db()[ns["strategy_ids"][i].as<int>()]);
  }

  result->peak_after = ns["peak_after"].as<int>();
  //    std::cout << result->to_string() << std::endl;

  return result;
}

IStrategy*
StrategyBuilder::buildNovelDrugSwitchingStrategy(const YAML::Node &ns, const int strategy_id, Config* config) {
  auto* result = new NovelDrugSwitchingStrategy();
  result->id = strategy_id;
  result->name = ns["name"].as<std::string>();

  add_distributions(ns["distribution"], result->distribution);
  add_therapies(ns, result, config);

  result->switch_to = ns["switch_to"].as<int>();
  result->tf_threshold = ns["tf_threshold"].as<double>();

  return result;
}
