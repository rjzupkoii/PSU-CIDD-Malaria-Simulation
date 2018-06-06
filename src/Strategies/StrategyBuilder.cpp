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
#include "../Config.h"
#include "../Model.h"
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

StrategyBuilder::StrategyBuilder() {
}

StrategyBuilder::~StrategyBuilder() {
}

IStrategy *StrategyBuilder::build(const YAML::Node &ns, const int &strategy_id) {
    IStrategy::StrategyType type = IStrategy::StrategyTypeMap[ns["type"].as<std::string>()];
    switch (type) {
        case IStrategy::SFT:
            return buildSFTStrategy(ns, strategy_id);
        case IStrategy::Cycling:
            return buildCyclingStrategy(ns, strategy_id);
        case IStrategy::AdaptiveCycling:
            return buildAdaptiveCyclingStrategy(ns, strategy_id);
        case IStrategy::MFT:
            return buildMFTStrategy(ns, strategy_id);
        case IStrategy::ACTIncreaseOvertime:
            return buildACTIncreaseStrategy(ns, strategy_id);
        case IStrategy::NovelNonACTSwitching:
            return buildNovelNonACTSwitchingStrategy(ns, strategy_id);
        case IStrategy::TACTSwitching:
            return buildTACTSwitchingStrategy(ns, strategy_id);
        case IStrategy::MFTRebalancing:
            return buildMFTRebalancingStrategy(ns, strategy_id);
        case IStrategy::NestedSwitching:
            return buildNestedSwitchingStrategy(ns, strategy_id);
        case IStrategy::MFTDifferentDistributionByLocation:
            return buildMFTDifferentDistributionByLocationStrategy(ns, strategy_id);
        case IStrategy::NestedSwitchingDifferentDistributionByLocation:
            return buildNestedSwitchingDifferentDistributionByLocationStrategy(ns, strategy_id);
        default:
            return nullptr;
    }
}

void StrategyBuilder::add_therapies(const YAML::Node &ns, IStrategy *&result) {
    for (int i = 0; i < ns["therapy_ids"].size(); i++) {
        result->add_therapy(Model::CONFIG->therapy_db()[ns["therapy_ids"][i].as<int>()]);
    }
}

void StrategyBuilder::add_distributions(const YAML::Node &ns, DoubleVector &v) {
    for (int i = 0; i < ns.size(); i++) {
        v.push_back(ns[i].as<double>());
    }
}

IStrategy *StrategyBuilder::buildSFTStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new SFTStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();
    result->add_therapy(Model::CONFIG->therapy_db()[ns["therapy_id"].as<int>()]);
    return result;
}

IStrategy *StrategyBuilder::buildCyclingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new CyclingStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    ((CyclingStrategy *) result)->set_cycling_time(ns["cycling_time"].as<int>());
    ((CyclingStrategy *) result)->set_next_switching_day(
            Model::CONFIG->start_treatment_day() + ns["cycling_time"].as<int>());

    add_therapies(ns, result);

    return result;
}

IStrategy *StrategyBuilder::buildAdaptiveCyclingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new AdaptiveCyclingStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    ((AdaptiveCyclingStrategy *) result)->set_trigger_value(ns["trigger_value"].as<double>());
    ((AdaptiveCyclingStrategy *) result)->set_delay_until_actual_trigger(ns["delay_until_actual_trigger"].as<int>());
    ((AdaptiveCyclingStrategy *) result)->set_turn_off_days(ns["turn_off_days"].as<int>());

    add_therapies(ns, result);
    return result;
}

IStrategy *StrategyBuilder::buildMFTStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new MFTStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    add_distributions(ns["distribution"], ((MFTStrategy *) result)->distribution());
    add_therapies(ns, result);
    return result;
}

IStrategy *StrategyBuilder::buildACTIncreaseStrategy(const YAML::Node &ns, const int &strategy_id) {

    IStrategy *result = new ACTIncreaseStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    add_distributions(ns["start_distribution"], ((ACTIncreaseStrategy *) result)->start_distribution());
    add_distributions(ns["start_distribution"], ((ACTIncreaseStrategy *) result)->distribution());
    add_distributions(ns["end_distribution"], ((ACTIncreaseStrategy *) result)->end_distribution());

    add_therapies(ns, result);

    return result;
}

IStrategy *StrategyBuilder::buildNovelNonACTSwitchingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new NovelNonACTSwitchingStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    add_distributions(ns["distribution"], ((MFTStrategy *) result)->distribution());
    add_therapies(ns, result);

    //     non_artemisinin_switching_day: -1
    //        non_art_therapy_id: 3
    //        fraction_non_art_replacement: 0.33333
    ((NovelNonACTSwitchingStrategy *) result)->set_non_artemisinin_switching_day(
            ns["non_artemisinin_switching_day"].as<int>());
    ((NovelNonACTSwitchingStrategy *) result)->set_non_art_therapy_id(ns["non_art_therapy_id"].as<int>());
    ((NovelNonACTSwitchingStrategy *) result)->set_fraction_non_art_replacement(
            ns["fraction_non_art_replacement"].as<double>());

    Model::CONFIG->set_start_intervention_day(
            ((NovelNonACTSwitchingStrategy *) result)->non_artemisinin_switching_day());

    return result;
}

IStrategy *StrategyBuilder::buildTACTSwitchingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new TACTSwitchingTStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();


    add_distributions(ns["start_distribution"], ((ACTIncreaseStrategy *) result)->start_distribution());
    add_distributions(ns["start_distribution"], ((ACTIncreaseStrategy *) result)->distribution());
    add_distributions(ns["end_distribution"], ((ACTIncreaseStrategy *) result)->end_distribution());

    add_therapies(ns, result);

    ((TACTSwitchingTStrategy *) result)->set_TACT_switching_day(ns["TACT_switching_day"].as<int>());
    ((TACTSwitchingTStrategy *) result)->set_TACT_id(ns["TACT_id"].as<int>());


    return result;
}

IStrategy *StrategyBuilder::buildNestedSwitchingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new NestedSwitchingStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    add_distributions(ns["start_distribution"], ((NestedSwitchingStrategy *) result)->start_distribution());
    add_distributions(ns["start_distribution"], ((NestedSwitchingStrategy *) result)->distribution());
    add_distributions(ns["end_distribution"], ((NestedSwitchingStrategy *) result)->end_distribution());

    for (int i = 0; i < ns["strategy_ids"].size(); i++) {
        ((NestedSwitchingStrategy *) result)->add_strategy(
                Model::CONFIG->strategy_db()[ns["strategy_ids"][i].as<int>()]);
    }

    ((NestedSwitchingStrategy *) result)->set_strategy_switching_day(ns["strategy_switching_day"].as<int>());
    ((NestedSwitchingStrategy *) result)->set_switch_to_strategy_id(ns["switch_to_strategy_id"].as<int>());


    return result;
}

IStrategy *StrategyBuilder::buildMFTRebalancingStrategy(const YAML::Node &ns, const int &strategy_id) {
    IStrategy *result = new MFTRebalancingStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    add_distributions(ns["distribution"], ((MFTRebalancingStrategy *) result)->distribution());
    add_distributions(ns["distribution"], ((MFTRebalancingStrategy *) result)->next_distribution());

    add_therapies(ns, result);

    ((MFTRebalancingStrategy *) result)->set_update_duration_after_rebalancing(
            ns["update_duration_after_rebalancing"].as<int>());
    ((MFTRebalancingStrategy *) result)->set_delay_until_actual_trigger(ns["delay_until_actual_trigger"].as<int>());
    ((MFTRebalancingStrategy *) result)->set_latest_adjust_distribution_time(Model::CONFIG->start_treatment_day());

    return result;
}

IStrategy *
StrategyBuilder::buildMFTDifferentDistributionByLocationStrategy(const YAML::Node &ns, const int &strategy_id) {
    auto *result = new MFTDifferentDistributionByLocationStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    result->distribution_by_location().clear();
    result->distribution_by_location().resize(
            static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        int input_loc = ns["distribution"].size() < Model::CONFIG->number_of_locations() ? 0 : loc;
        add_distributions(ns["distribution"][input_loc], result->distribution_by_location()[loc]);
    }

    add_therapies(ns, reinterpret_cast<IStrategy *&>(result));

    return result;
}

IStrategy *StrategyBuilder::buildNestedSwitchingDifferentDistributionByLocationStrategy(const YAML::Node &ns,
                                                                                        const int &strategy_id) {
    auto *result = new NestedSwitchingDifferentDistributionByLocationStrategy();
    result->id = strategy_id;
    result->name = ns["name"].as<std::string>();

    result->distribution().clear();
    result->distribution().resize(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));

    result->start_distribution().clear();
    result->start_distribution().resize(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        int input_loc = ns["start_distribution"].size() < Model::CONFIG->number_of_locations() ? 0 : loc;
        add_distributions(ns["start_distribution"][input_loc], result->distribution()[loc]);
    }
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        int input_loc = ns["start_distribution"].size() < Model::CONFIG->number_of_locations() ? 0 : loc;
        add_distributions(ns["start_distribution"][input_loc], result->start_distribution()[loc]);
    }

    for (int i = 0; i < ns["strategy_ids"].size(); i++) {
        result->add_strategy(Model::CONFIG->strategy_db()[ns["strategy_ids"][i].as<int>()]);
    }

    result->set_peak_at(ns["peak_at"].as<int>());
    result->set_strategy_switching_day(ns["strategy_switching_day"].as<int>());
    result->set_switch_to_strategy_id(ns["switch_to_strategy_id"].as<int>());

//    std::cout << result->to_string() << std::endl;

    return result;
}
