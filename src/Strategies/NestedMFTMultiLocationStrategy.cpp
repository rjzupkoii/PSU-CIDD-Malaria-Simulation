//
// Created by Nguyen Tran on 3/17/2018.
//

#include <sstream>
#include "NestedMFTMultiLocationStrategy.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Therapies/Therapy.hxx"



NestedMFTMultiLocationStrategy::NestedMFTMultiLocationStrategy() : IStrategy(
    "NestedMFTMultiLocationStrategy", NestedMFTMultiLocation) {}

NestedMFTMultiLocationStrategy::~NestedMFTMultiLocationStrategy() = default;

void NestedMFTMultiLocationStrategy::add_strategy(IStrategy *strategy) {
  strategy_list.push_back(strategy);
}

void NestedMFTMultiLocationStrategy::add_therapy(Therapy *therapy) {}

Therapy *NestedMFTMultiLocationStrategy::get_therapy(Person *person) {
  const auto loc = person->location();
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (std::size_t i = 0; i < distribution[loc].size(); i++) {
    sum += distribution[loc][i];
    if (p <= sum) {
      return strategy_list[i]->get_therapy(person);
    }
  }
  return strategy_list[strategy_list.size() - 1]->get_therapy(person);
}

std::string NestedMFTMultiLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << std::endl;

  // Determine the distribution size
  if (distribution.size() == 0) {
    sstm << "NOT_SET";
  } else {
    for (auto i : distribution[0]) {
      sstm << i << ",";
    }
  } 
  sstm << std::endl;

  // Determine the start of the distribution
  if (start_distribution.size() == 0) {
    sstm << "NOT_SET";
  } else {
    for (auto i : start_distribution[0]) {
      sstm << i << ",";
    }
  }
  sstm << std::endl;

  return sstm.str();
}

void NestedMFTMultiLocationStrategy::update_end_of_time_step() {
  // update each strategy in the nest
  for (auto &strategy : strategy_list) {
    strategy->update_end_of_time_step();
  }
}

void NestedMFTMultiLocationStrategy::adjust_distribution(const int &time) {
  if (peak_after==-1) {
    // inflation every year
    for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      const auto d_act = distribution[loc][0]*(1 + Model::CONFIG->inflation_factor()/12);
      distribution[loc][0] = d_act;
      const auto other_d = (1 - d_act)/(distribution[loc].size() - 1);
      for (std::size_t i = 1; i < distribution[loc].size(); i++) {
        distribution[loc][i] = other_d;
      }
    }
  } else {
    // increasing linearly
    if (time <= starting_time + peak_after) {
      if (distribution[0][0] < 1) {
        for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
          for (std::size_t i = 0; i < distribution[loc].size(); i++) {
            const auto dist =
                (peak_distribution[loc][i] - start_distribution[loc][i])*(time - starting_time)/
                    peak_after + start_distribution[
                    loc][i];
            distribution[loc][i] = dist;
          }
        }
      }
    }
  } //    std::cout << to_string() << std::endl;
}

void NestedMFTMultiLocationStrategy::adjust_started_time_point(const int &current_time) {
  starting_time = current_time;
  // update each strategy in the nest
  for (auto *strategy : strategy_list) {
    strategy->adjust_started_time_point(current_time);
  }
}

void NestedMFTMultiLocationStrategy::monthly_update() {
  adjust_distribution(Model::SCHEDULER->current_time());

  for (auto *strategy : strategy_list) {
    strategy->monthly_update();
  }

  // for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
  //   std::cout << distribution[loc] << std::endl;
  // }

}
