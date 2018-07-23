/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ACTIncreaseStrategy.cpp
 * Author: Merlin
 * 
 * Created on April 26, 2017, 11:20 AM
 */

#include "ACTIncreaseStrategy.h"
#include "Core/Random.h"
#include "Model.h"
#include <cassert>
#include "Therapies/Therapy.h"
#include "Core/Config/Config.h"
#include <sstream>

ACTIncreaseStrategy::ACTIncreaseStrategy(): IStrategy("ACTIncreaseStrategy", ACTIncreaseOvertime) {}

ACTIncreaseStrategy::~ACTIncreaseStrategy() = default;

void ACTIncreaseStrategy::add_therapy(Therapy* therapy) {
  therapy_list.push_back(therapy);
}

Therapy* ACTIncreaseStrategy::get_therapy(Person* person) {
  auto P = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (auto i = 0; i < distribution.size(); i++) {
    sum += distribution[i];
    if (P <= sum) {
      return therapy_list[i];
    }
  }

  return therapy_list[therapy_list.size() - 1];
}

std::string ACTIncreaseStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-";
  for (auto i = 0; i < therapy_list.size() - 1; i++) {
    sstm << therapy_list[i]->id() << ",";
  }
  sstm << therapy_list[therapy_list.size() - 1]->id() << "-";

  for (auto i = 0; i < start_distribution.size() - 1; i++) {
    sstm << start_distribution[i] << ",";
  }
  sstm << start_distribution[therapy_list.size() - 1] << "-";


  for (int i = 0; i < end_distribution.size() - 1; i++) {
    sstm << end_distribution[i] << ",";
  }
  sstm << end_distribution[therapy_list.size() - 1] << "-";

  for (int i = 0; i < distribution.size() - 1; i++) {
    sstm << distribution[i] << ",";
  }
  sstm << distribution[therapy_list.size() - 1];

  return sstm.str();
}

void ACTIncreaseStrategy::update_end_of_time_step() {
  if (Model::SCHEDULER->current_time() % 30 == 0) {
    adjust_disttribution(Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
    //        std::cout << to_string() << std::endl;
  }
}

void ACTIncreaseStrategy::adjust_disttribution(const int time, const int totaltime) {

  const auto dACT = ((end_distribution[0] - start_distribution[0]) * time) / totaltime + start_distribution[0];

  distribution[0] = dACT;
  const auto otherD = (1 - dACT) / (distribution.size() - 1);
  for (auto i = 1; i < distribution.size(); i++) {
    distribution[i] = otherD;
  }
}

void ACTIncreaseStrategy::adjust_started_time_point(const int& current_time) {}
void ACTIncreaseStrategy::monthly_update() {}

