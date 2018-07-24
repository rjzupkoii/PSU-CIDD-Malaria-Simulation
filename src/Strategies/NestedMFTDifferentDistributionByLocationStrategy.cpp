//
// Created by Nguyen Tran on 3/17/2018.
//

#include <sstream>
#include "NestedMFTDifferentDistributionByLocationStrategy.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Person.h"
#include "Scheduler.h"
#include "Therapies/Therapy.h"
#include "MFTRebalancingStrategy.h"
#include "CyclingStrategy.h"
#include "AdaptiveCyclingStrategy.h"
#include "Constants.h"
#include "NestedMFTStrategy.h"

// TODO: check if it match with the calendar day

NestedMFTDifferentDistributionByLocationStrategy::NestedMFTDifferentDistributionByLocationStrategy(): IStrategy(
  "NestedMFTDifferentDistributionByLocationStrategy", NestedMFTDifferentDistributionByLocation) {}

NestedMFTDifferentDistributionByLocationStrategy::~NestedMFTDifferentDistributionByLocationStrategy() = default;

void NestedMFTDifferentDistributionByLocationStrategy::add_strategy(IStrategy* strategy) {
  strategy_list.push_back(strategy);
}

void NestedMFTDifferentDistributionByLocationStrategy::add_therapy(Therapy* therapy) {}

Therapy* NestedMFTDifferentDistributionByLocationStrategy::get_therapy(Person* person) {
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

std::string NestedMFTDifferentDistributionByLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << std::endl;


  for (auto i : distribution[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;

  for (auto i : start_distribution[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;
  return sstm.str();
}

void NestedMFTDifferentDistributionByLocationStrategy::update_end_of_time_step() {  
  // update each strategy in the nest
  for (auto& strategy : strategy_list) {
    strategy->update_end_of_time_step();
  }
}

void NestedMFTDifferentDistributionByLocationStrategy::adjust_distribution(const int& time) {
  if (peak_after == -1) {
    // inflation every year
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      const auto d_act = distribution[loc][0] * (1 + Model::CONFIG->inflation_factor()/12);
      distribution[loc][0] = d_act;
      const auto other_d = (1 - d_act) / (distribution[loc].size() - 1);
      for (auto i = 1; i < distribution[loc].size(); i++) {
        distribution[loc][i] = other_d;
      }
    }
  }
  else {
    // increasing linearly
    if (time <= starting_time + peak_after) {
      if (distribution[0][0] < 1) {
        for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
          auto d_act = (peak_distribution[loc][0] - start_distribution[loc][0]) * (time - starting_time) / peak_after + start_distribution[
            loc][0];
          d_act = d_act >= 1 ? 1 : d_act;
          distribution[loc][0] = d_act;
          const auto other_d = (1 - d_act) / (distribution[loc].size() - 1);
          for (auto i = 1; i < distribution[loc].size(); i++) {
            distribution[loc][i] = other_d;
          }
        }
      }
    }
  } //    std::cout << to_string() << std::endl;
}


void NestedMFTDifferentDistributionByLocationStrategy::adjust_started_time_point(const int& current_time) {
  starting_time = current_time;
  // update each strategy in the nest
  for (auto& strategy : strategy_list) {
    strategy->adjust_started_time_point(current_time);
  }
}

void NestedMFTDifferentDistributionByLocationStrategy::monthly_update() {
  adjust_distribution(Model::SCHEDULER->current_time());
  // std::cout << distribution[0] << "-" << distribution[1] << std::endl;
}
