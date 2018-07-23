//
// Created by Nguyen Tran on 3/17/2018.
//

#include <sstream>
#include "NestedSwitchingDifferentDistributionByLocationStrategy.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Person.h"
#include "Scheduler.h"
#include "Therapies/Therapy.h"
#include "MFTRebalancingStrategy.h"
#include "CyclingStrategy.h"
#include "AdaptiveCyclingStrategy.h"
#include "NestedSwitchingStrategy.h"
#include "Constants.h"

// TODO: check if it match with the calendar day

NestedSwitchingDifferentDistributionByLocationStrategy::NestedSwitchingDifferentDistributionByLocationStrategy(): IStrategy(
  "NestedSwitchingDifferentDistributionByLocationStrategy", NestedSwitchingDifferentDistributionByLocation) {}

NestedSwitchingDifferentDistributionByLocationStrategy::~NestedSwitchingDifferentDistributionByLocationStrategy() = default;

void NestedSwitchingDifferentDistributionByLocationStrategy::add_strategy(IStrategy* strategy) {
  strategy_list.push_back(strategy);
}

void NestedSwitchingDifferentDistributionByLocationStrategy::add_therapy(Therapy* therapy) {}

Therapy* NestedSwitchingDifferentDistributionByLocationStrategy::get_therapy(Person* person) {
  const auto loc = person->location();
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (auto i = 0; i < distribution[loc].size(); i++) {
    sum += distribution[loc][i];
    if (p <= sum) {
      return strategy_list[i]->get_therapy(person);
    }
  }
  return strategy_list[strategy_list.size() - 1]->get_therapy(person);
}

std::string NestedSwitchingDifferentDistributionByLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-" << " switch to " << switch_to_strategy_id << " at "
    << strategy_switching_day << std::endl;


  for (double i : distribution[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;

  for (double i : start_distribution[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;
  return sstm.str();
}

void NestedSwitchingDifferentDistributionByLocationStrategy::update_end_of_time_step() {
  if (Model::SCHEDULER->current_time() == strategy_switching_day) {
    //        std::cout << to_string() << std::endl;
    strategy_list[0] = Model::CONFIG->strategy_db()[switch_to_strategy_id];

    if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::NestedSwitching) {
      static_cast<NestedSwitchingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id])->adjustDisttribution(
        Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
    }
  }

  adjust_distribution(Model::SCHEDULER->current_time(), peak_at);

  // update each strategy in the nest
  for (auto& strategy : strategy_list) {
    strategy->update_end_of_time_step();
  }
}

void NestedSwitchingDifferentDistributionByLocationStrategy::adjust_distribution(int time, int peak_at) {
  //TODO: rework here to remove start_intervention_day
  if (time > Model::CONFIG->start_intervention_day() &&
    (time - Model::CONFIG->start_intervention_day()) % Constants::DAYS_IN_YEAR() == 0) {
    if (peak_at == -1) {
      // inflation every year
      for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        const auto dACT = distribution[loc][0] * Model::CONFIG->inflation_factor();
        distribution[loc][0] = dACT;
        const auto otherD = (1 - dACT) / (distribution[loc].size() - 1);
        for (auto i = 1; i < distribution[loc].size(); i++) {
          distribution[loc][i] = otherD;
        }
      }
    }
    else {
      // increasing linearly
      if (distribution[0][0] < 1) {
        for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
          auto dACT = ((1 - start_distribution[loc][0]) * time) / peak_at + start_distribution[loc][0];
          dACT = dACT >= 1 ? 1 : dACT;
          distribution[loc][0] = dACT;
          const auto otherD = (1 - dACT) / (distribution[loc].size() - 1);
          for (auto i = 1; i < distribution[loc].size(); i++) {
            distribution[loc][i] = otherD;
          }
        }
      }
    }
  }

  //    std::cout << to_string() << std::endl;
}


void NestedSwitchingDifferentDistributionByLocationStrategy::adjust_started_time_point(const int& current_time) {
  // when switch to MFTBalancing
  if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::MFTRebalancing) {
    //        std::cout << "hello" << std::endl;
    auto* s = dynamic_cast<MFTRebalancingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id]);
    s->next_update_time = current_time + 60;
    s->latest_adjust_distribution_time = -1;
  }
  // when switch to Cycling
  if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::Cycling) {
    auto* s = dynamic_cast<CyclingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id]);
    //        std::cout << "hello" << std::endl;
    s->next_switching_day = current_time + s->cycling_time;
  }
  // when switch to AdaptiveCycling
  if (Model::CONFIG->strategy_db()[switch_to_strategy_id]->get_type() == IStrategy::AdaptiveCycling) {
    auto* s = dynamic_cast<AdaptiveCyclingStrategy *>(Model::CONFIG->strategy_db()[switch_to_strategy_id]);
    s->latest_switch_time = current_time;
    s->index = -1;
  }
}

void NestedSwitchingDifferentDistributionByLocationStrategy::monthly_update() {}
