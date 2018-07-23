//
// Created by Nguyen Tran on 3/16/2018.
//

#include <sstream>
#include "MFTDifferentDistributionByLocationStrategy.h"
#include "Therapies/Therapy.h"
#include "Model.h"
#include "Core/Random.h"
#include "Person.h"
#include "Core/Config/Config.h"

MFTDifferentDistributionByLocationStrategy::MFTDifferentDistributionByLocationStrategy(): IStrategy(
  "MFTDifferentDistributionByLocationStrategy", MFTDifferentDistributionByLocation) {}

MFTDifferentDistributionByLocationStrategy::~MFTDifferentDistributionByLocationStrategy() = default;

void MFTDifferentDistributionByLocationStrategy::add_therapy(Therapy* therapy) {
  therapy_list.push_back(therapy);
}

Therapy* MFTDifferentDistributionByLocationStrategy::get_therapy(Person* person) {

  const auto p = Model::RANDOM->random_flat(0.0, 1.0);
  const auto loc = person->location();

  double sum = 0;
  for (auto i = 0; i < distribution_by_location[loc].size(); i++) {
    sum += distribution_by_location[loc][i];
    if (p <= sum) {
      return therapy_list[i];
    }
  }

  return therapy_list[therapy_list.size() - 1];
}

std::string MFTDifferentDistributionByLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-";

  for (auto i = 0; i < therapy_list.size() - 1; i++) {
    std::cout << "hello" << therapy_list[i]->id() << std::endl;
  }
  sstm << therapy_list[therapy_list.size() - 1]->id() << "-" << std::endl;;

  for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    sstm << "[";
    for (int i = 0; i < distribution_by_location[loc].size() - 1; i++) {
      sstm << distribution_by_location[loc][i] << ",";
    }
    sstm << distribution_by_location[loc][therapy_list.size() - 1] << "]" << std::endl;
  }

  return sstm.str();
}

void MFTDifferentDistributionByLocationStrategy::update_end_of_time_step() {
  //do nothing here
}

void MFTDifferentDistributionByLocationStrategy::adjust_started_time_point(const int& current_time) {}
void MFTDifferentDistributionByLocationStrategy::monthly_update() {}
