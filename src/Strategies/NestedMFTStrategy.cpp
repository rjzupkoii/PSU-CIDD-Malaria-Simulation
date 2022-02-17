#include "NestedMFTStrategy.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Therapies/Therapy.hxx"

void NestedMFTStrategy::add_strategy(IStrategy *strategy) {
  strategy_list.push_back(strategy);
}

void NestedMFTStrategy::add_therapy(Therapy *therapy) {}

Therapy *NestedMFTStrategy::get_therapy(Person *person) {
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (std::size_t i = 0; i < distribution.size(); i++) {
    sum += distribution[i];
    if (p <= sum) {
      return strategy_list[i]->get_therapy(person);
    }
  }
  return strategy_list[strategy_list.size() - 1]->get_therapy(person);
}

std::string NestedMFTStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";

  for (auto i : distribution) {
    sstm << i << "::";
  }
  return sstm.str();
}

void NestedMFTStrategy::adjust_started_time_point(const int &current_time) {
  // update each strategy in the nest
  for (auto &strategy : strategy_list) {
    strategy->adjust_started_time_point(current_time);
  }
  starting_time = current_time;
}

void NestedMFTStrategy::update_end_of_time_step() {
  // update each strategy in the nest
  for (auto *strategy : strategy_list) {
    strategy->update_end_of_time_step();
  }
}

void NestedMFTStrategy::monthly_update() {
  adjust_distribution(Model::SCHEDULER->current_time());

  for (auto *strategy : strategy_list) {
    strategy->monthly_update();
  }
  // std::cout << distribution[0] << "-" << distribution[1] << std::endl;
}

void NestedMFTStrategy::adjust_distribution(const int &time) {
  if (time < starting_time + peak_after) {

    for (std::size_t i = 0; i < distribution.size(); i++) {
      const auto dist = (peak_distribution[i] - start_distribution[i])*(time - starting_time)/peak_after +
          start_distribution[i];
      distribution[i] = dist;
    }
  }
}
