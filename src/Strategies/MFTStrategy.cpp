/* 
 * File:   MFTStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 4, 2013, 11:09 AM
 */

#include "MFTStrategy.h"
#include "Core/Random.h"
#include "Model.h"
#include <sstream>
#include "IStrategy.h"
#include "Therapies/Therapy.hxx"

MFTStrategy::MFTStrategy() : IStrategy("MFTStrategy", MFT) {}

MFTStrategy::~MFTStrategy() = default;

void MFTStrategy::add_therapy(Therapy *therapy) {
  therapy_list.push_back(therapy);
}

Therapy *MFTStrategy::get_therapy(Person *person) {

  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (std::size_t i = 0; i < distribution.size(); i++) {
    sum += distribution[i];
    if (p <= sum) {
      return therapy_list[i];
    }
  }

  return therapy_list[therapy_list.size() - 1];
}

std::string MFTStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto *therapy : therapy_list) {
    sstm << sep << therapy->id();
    sep = ",";
  }
  sep = "";
  sstm << "-";
  for (auto dist : distribution) {
    sstm << sep << dist;
    sep = ",";
  }
  return sstm.str();
}

void MFTStrategy::adjust_started_time_point(const int &current_time) {}

void MFTStrategy::monthly_update() {
  //do nothing here

}

void MFTStrategy::update_end_of_time_step() {
  //do nothing here
}
