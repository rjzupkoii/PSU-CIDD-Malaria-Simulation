/* 
 * File:   AdaptiveCyclingStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 4, 2013, 11:10 AM
 */

#include "AdaptiveCyclingStrategy.h"
#include "Model.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"
#include <sstream>
#include "IStrategy.h"
#include "Therapies/Therapy.hxx"

AdaptiveCyclingStrategy::AdaptiveCyclingStrategy() : IStrategy("AdaptiveCyclingStrategy", AdaptiveCycling) {}

AdaptiveCyclingStrategy::~AdaptiveCyclingStrategy() = default;

void AdaptiveCyclingStrategy::add_therapy(Therapy *therapy) {
  therapy_list.push_back(therapy);
}

void AdaptiveCyclingStrategy::switch_therapy() {
  //    std::cout << "Switch from: " << index_ << "\t - to: " << index_ + 1;
  index++;
  index %= therapy_list.size();

  Model::DATA_COLLECTOR->update_UTL_vector();
  LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date}
            << ": Cycling Strategy Swith Therapy to: " << therapy_list[index]->id();
}

Therapy *AdaptiveCyclingStrategy::get_therapy(Person *person) {
  return therapy_list[index];
}

std::string AdaptiveCyclingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto *therapy : therapy_list) {
    sstm << sep << therapy->id();
    sep = ",";
  }
  return sstm.str();
}

void AdaptiveCyclingStrategy::update_end_of_time_step() {

  if (Model::SCHEDULER->current_time()==latest_switch_time) {
    switch_therapy();
    //            std::cout << to_string() << std::endl;
  } else {
    if (Model::DATA_COLLECTOR->current_tf_by_therapy()[get_therapy(nullptr)->id()] > trigger_value) {
      // TODO:: turn_off_days and delay_until_actual_trigger should be match with calendar day
      if (Model::SCHEDULER->current_time() > latest_switch_time + turn_off_days) {
        latest_switch_time = Model::SCHEDULER->current_time() + delay_until_actual_trigger;
        LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date}
                  << ": Adaptive Cyling will switch therapy next year";
        //                    std::cout << "TF: " << Model::DATA_COLLECTOR->current_TF_by_therapy()[get_therapy()->id()] << std::endl;
      }
    }
  }

}

void AdaptiveCyclingStrategy::adjust_started_time_point(const int &current_time) {
  latest_switch_time = -1;
  index = 0;
}

void AdaptiveCyclingStrategy::monthly_update() {}
