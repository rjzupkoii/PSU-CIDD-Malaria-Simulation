/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NestedSwitchingStrategy.cpp
 * Author: Merlin
 * 
 * Created on August 25, 2017, 11:07 AM
 */

#include "NestedSwitchingStrategy.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include "Core/Random.h"
#include "Scheduler.h"
#include "MFTRebalancingStrategy.h"
#include "CyclingStrategy.h"
#include "AdaptiveCyclingStrategy.h"
#include "Therapies/Therapy.h"


// TODO: check if it match with calendar day

NestedSwitchingStrategy::NestedSwitchingStrategy(): IStrategy("NestedSwitchingStrategy", NestedSwitching) {}

NestedSwitchingStrategy::~NestedSwitchingStrategy() = default;

void NestedSwitchingStrategy::add_strategy(IStrategy* strategy) {
  strategy_list.push_back(strategy);
}

Therapy* NestedSwitchingStrategy::get_therapy(Person* person) {
  double P = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (int i = 0; i < distribution.size(); i++) {
    sum += distribution[i];
    if (P <= sum) {
      return strategy_list[i]->get_therapy(person);
    }
  }
  return strategy_list[strategy_list.size() - 1]->get_therapy(person);
}

void NestedSwitchingStrategy::add_therapy(Therapy* therapy) {}

IStrategy::StrategyType NestedSwitchingStrategy::get_type() const {
  return IStrategy::NestedSwitching;
}

std::string NestedSwitchingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-";

  for (int i = 0; i < distribution.size(); i++) {
    sstm << distribution[i] << "::";
  }
  return sstm.str();
}

void NestedSwitchingStrategy::update_end_of_time_step() {
  if (Model::SCHEDULER->current_time() == strategy_switching_day) {
    //        std::cout << to_string() << std::endl;
    strategy_list[0] = Model::CONFIG->strategy_db()[switch_to_strategy_id];

    if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::NestedSwitching) {
      dynamic_cast<NestedSwitchingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id])->adjustDisttribution(
        Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
    }
    //        std::cout << to_string() << std::endl;
  }

  if (Model::SCHEDULER->current_time() % 30 == 0) {
    adjustDisttribution(Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
    //        std::cout << to_string() << std::endl;
  }
  // update each strategy in the nest
  for (auto& strategy : strategy_list) {
    strategy->update_end_of_time_step();
  }
}

void NestedSwitchingStrategy::adjustDisttribution(int time, int totaltime) {
  const double dACT = ((end_distribution[0] - start_distribution[0]) * time) / totaltime + start_distribution[0];

  distribution[0] = dACT;
  const double otherD = (1 - dACT) / (distribution.size() - 1);
  for (int i = 1; i < distribution.size(); i++) {
    distribution[i] = otherD;
  }
  //    std::cout << to_string() << std::endl;
}


void NestedSwitchingStrategy::adjust_started_time_point(const int& current_time) {
  // when switch to MFTBalancing
  if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::MFTRebalancing) {
    auto* s = dynamic_cast<MFTRebalancingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id]);
    //        std::cout << "hello" << std::endl;
    s->next_update_time = -1;
    s->latest_adjust_distribution_time = current_time;
  }

}
void NestedSwitchingStrategy::monthly_update() {}
