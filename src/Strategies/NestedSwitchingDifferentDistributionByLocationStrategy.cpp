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

NestedSwitchingDifferentDistributionByLocationStrategy::NestedSwitchingDifferentDistributionByLocationStrategy()
  : strategy_switching_day_{-1}, switch_to_strategy_id_{-1}, peak_at_{-1} {};

NestedSwitchingDifferentDistributionByLocationStrategy::~NestedSwitchingDifferentDistributionByLocationStrategy() = default;

void NestedSwitchingDifferentDistributionByLocationStrategy::add_strategy(IStrategy* strategy) {
  strategy_list_.push_back(strategy);
}

void NestedSwitchingDifferentDistributionByLocationStrategy::add_therapy(Therapy* therapy) {}

Therapy* NestedSwitchingDifferentDistributionByLocationStrategy::get_therapy(Person* person) {
  int loc = person->location();
  double P = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (int i = 0; i < distribution_[loc].size(); i++) {
    sum += distribution_[loc][i];
    if (P <= sum) {
      return strategy_list_[i]->get_therapy(person);
    }
  }
  return strategy_list_[strategy_list_.size() - 1]->get_therapy(person);
}

std::string NestedSwitchingDifferentDistributionByLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-" << " switch to " << switch_to_strategy_id_ << " at "
    << strategy_switching_day_ << std::endl;


  for (double i : distribution_[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;

  for (double i : start_distribution_[Model::CONFIG->number_of_locations() - 1]) {
    sstm << i << ",";
  }
  sstm << std::endl;
  return sstm.str();
}

IStrategy::StrategyType NestedSwitchingDifferentDistributionByLocationStrategy::get_type() const {
  return IStrategy::NestedSwitchingDifferentDistributionByLocation;
}

void NestedSwitchingDifferentDistributionByLocationStrategy::update_end_of_time_step() {
  if (Model::SCHEDULER->current_time() == strategy_switching_day_) {
    //        std::cout << to_string() << std::endl;
    strategy_list_[0] = Model::CONFIG->strategy_db()[switch_to_strategy_id_];


    if (Model::CONFIG->strategy_db()[switch_to_strategy_id_]->get_type() == IStrategy::NestedSwitching) {
      ((NestedSwitchingStrategy *)Model::CONFIG->strategy_db()[switch_to_strategy_id_])->adjustDisttribution(
        Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
    }
  }

  adjustDistribution(Model::SCHEDULER->current_time(), peak_at_);

  // update each strategy in the nest
  for (auto& strategy : strategy_list_) {
    strategy->update_end_of_time_step();
  }
}

void NestedSwitchingDifferentDistributionByLocationStrategy::adjustDistribution(int time, int peak_at) {

  if (time > Model::CONFIG->start_intervention_day() &&
    (time - Model::CONFIG->start_intervention_day()) % Constants::DAYS_IN_YEAR() == 0) {
    if (peak_at == -1) {
      // inflation every year
      for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        double dACT = distribution_[loc][0] * Model::CONFIG->inflation_factor();
        distribution_[loc][0] = dACT;
        double otherD = (1 - dACT) / (distribution_[loc].size() - 1);
        for (int i = 1; i < distribution_[loc].size(); i++) {
          distribution_[loc][i] = otherD;
        }
      }
    }
    else {
      // increasing linearly
      if (distribution_[0][0] < 1) {
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
          double dACT = ((1 - start_distribution_[loc][0]) * time) / peak_at_ + start_distribution_[loc][0];
          dACT = dACT >= 1 ? 1 : dACT;
          distribution_[loc][0] = dACT;
          double otherD = (1 - dACT) / (distribution_[loc].size() - 1);
          for (int i = 1; i < distribution_[loc].size(); i++) {
            distribution_[loc][i] = otherD;
          }
        }
      }
    }
  }

  //    std::cout << to_string() << std::endl;
}

void NestedSwitchingDifferentDistributionByLocationStrategy::initialize_update_time(Config* config) {
  // when switch to MFTBalancing
  if (config->strategy_db()[switch_to_strategy_id_]->get_type() == IStrategy::MFTRebalancing) {
    //        std::cout << "hello" << std::endl;
    auto* s = dynamic_cast<MFTRebalancingStrategy *>(config->strategy_db()[switch_to_strategy_id_]);
    s->set_next_update_time(strategy_switching_day_ + 60);
    s->set_latest_adjust_distribution_time(-1);
  }
  // when switch to Cycling
  if (config->strategy_db()[switch_to_strategy_id_]->get_type() == IStrategy::Cycling) {
    auto* s = dynamic_cast<CyclingStrategy *>(config->strategy_db()[switch_to_strategy_id_]);
    //        std::cout << "hello" << std::endl;
    s->set_next_switching_day(strategy_switching_day_ + s->cycling_time());
  }
  // when switch to AdaptiveCycling
  if (config->strategy_db()[switch_to_strategy_id_]->get_type() == IStrategy::AdaptiveCycling) {
    auto* s = dynamic_cast<AdaptiveCyclingStrategy *>(config->strategy_db()[switch_to_strategy_id_]);
    s->set_latest_switch_time(strategy_switching_day_);
    s->set_index(-1);
  }
}
