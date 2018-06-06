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

#include "../TypeDef.h"
#include "../PropertyMacro.h"
class IStrategy;

class StrategyBuilder {
    DISALLOW_COPY_AND_ASSIGN_(StrategyBuilder)
public:
    StrategyBuilder();
    virtual ~StrategyBuilder();

    static IStrategy* build(const YAML::Node& ns, const int& strategy_id);
    static void add_therapies(const YAML::Node& ns, IStrategy*& result);
    static void add_distributions(const YAML::Node& ns, DoubleVector& v);

    static IStrategy* buildSFTStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildCyclingStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildAdaptiveCyclingStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildMFTStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildMFTRebalancingStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildACTIncreaseStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildNovelNonACTSwitchingStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildTACTSwitchingStrategy(const YAML::Node& ns, const int& strategy_id);
    static IStrategy* buildNestedSwitchingStrategy(const YAML::Node& ns, const int& strategy_id);

private:

    static IStrategy *buildMFTDifferentDistributionByLocationStrategy(const YAML::Node &node, const int &id);

    static IStrategy *
    buildNestedSwitchingDifferentDistributionByLocationStrategy(const YAML::Node &ns, const int &strategy_id);
};

#endif /* STRATEGYBUILDER_H */

