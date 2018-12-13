/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StrategyBuilder.h
 * Author: Merlin
 *
 * Created on August 23, 2017, 11:03 AM
 */

#ifndef STRATEGYBUILDER_H
#define STRATEGYBUILDER_H

#include <yaml-cpp/yaml.h>

#include "Core/TypeDef.h"
#include "Core/PropertyMacro.h"

class IStrategy;

class Config;

class StrategyBuilder {
 DISALLOW_COPY_AND_ASSIGN(StrategyBuilder)

 public:
  StrategyBuilder();

  virtual ~StrategyBuilder();

  static IStrategy *build(const YAML::Node &ns, const int &strategy_id, Config *config);

  static void add_therapies(const YAML::Node &ns, IStrategy *result, Config *config);

  static void add_distributions(const YAML::Node &ns, DoubleVector &v);

  static IStrategy *buildSFTStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildCyclingStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildAdaptiveCyclingStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildMFTStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildMFTRebalancingStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildNestedSwitchingStrategy(const YAML::Node &ns, const int &strategy_id, Config *config);

  static IStrategy *buildMFTMultiLocationStrategy(const YAML::Node &node, const int &id, Config *config);

  static IStrategy *
  buildNestedMFTDifferentDistributionByLocationStrategy(const YAML::Node &ns, const int &strategy_id,
                                                        Config *config);

  static IStrategy *buildNovelDrugSwitchingStrategy(const YAML::Node &ns, const int strategy_id, Config *config);
};

#endif /* STRATEGYBUILDER_H */
