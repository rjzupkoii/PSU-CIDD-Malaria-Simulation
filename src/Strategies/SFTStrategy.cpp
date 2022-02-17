/* 
 * File:   SFTStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 8:00 PM
 */

#include <cassert>
#include "SFTStrategy.h"
#include "Therapies/Therapy.hxx"
#include "IStrategy.h"
#include <sstream>

SFTStrategy::SFTStrategy() : IStrategy("SFTStrategy", SFT) {}

SFTStrategy::~SFTStrategy() = default;

std::vector<Therapy *> &SFTStrategy::get_therapy_list() {
  return therapy_list_;
}

void SFTStrategy::add_therapy(Therapy *therapy) {
  therapy_list_.push_back(therapy);
}

Therapy *SFTStrategy::get_therapy(Person *person) {
  return therapy_list_[0];
}

std::string SFTStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-" << therapy_list_[0]->id();
  return sstm.str();
}

void SFTStrategy::update_end_of_time_step() {
  //do nothing here
}

void SFTStrategy::adjust_started_time_point(const int &current_time) {
  //do nothing
}

void SFTStrategy::monthly_update() {
  //do nothing
}
