/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SmartMFTStrategy.cpp
 * Author: Merlin
 * 
 * Created on August 25, 2017, 11:57 AM
 */

#include "MFTRebalancingStrategy.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Therapies/Therapy.hxx"
#include <string>

MFTRebalancingStrategy::MFTRebalancingStrategy() {
  name = "MFTRebalancingStrategy";
  type = MFTRebalancing;
}

MFTRebalancingStrategy::~MFTRebalancingStrategy() = default;

std::string MFTRebalancingStrategy::to_string() const {
  std::stringstream sstm;
  //    sstm << IStrategy::id << "-" << IStrategy::name << "-";
  //
  //    for (int i = 0; i < therapy_list().size() - 1; i++) {
  //        sstm << therapy_list()[i]->id() << ",";
  //    }
  //    sstm << therapy_list()[therapy_list().size() - 1]->id() << "-";
  //
  //    for (int i = 0; i < distribution().size() - 1; i++) {
  //        sstm << distribution()[i] << ",";
  //    }
  //    sstm << distribution()[therapy_list().size() - 1] << "-" << update_distribution_duration_;
  sstm << MFTStrategy::to_string() << "-" << update_duration_after_rebalancing;
  return sstm.str();
}

void MFTRebalancingStrategy::update_end_of_time_step() {

  if (Model::SCHEDULER->current_time()==latest_adjust_distribution_time) {
    // actual trigger adjust distribution
    for (std::size_t i = 0; i < distribution.size(); i++) {
      distribution[i] = next_distribution[i];
    }
    next_update_time = Model::SCHEDULER->current_time() + update_duration_after_rebalancing;
    LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date} << ": MFT Rebalancing adjust distribution: "
              << to_string();
    //            std::cout << to_string() << std::endl;
  } else {
    if (Model::SCHEDULER->current_time()==next_update_time) {
      double sum = 0;
      for (std::size_t i = 0; i < distribution.size(); i++) {
        LOG(INFO) << "Current treatment failure rate of " << therapy_list[i]->id() << " : "
                  << Model::DATA_COLLECTOR->
                      current_tf_by_therapy()[therapy_list[i]->id()];
        if (Model::DATA_COLLECTOR->current_tf_by_therapy()[therapy_list[i]->id()] < 0.05) {
          next_distribution[i] = 1.0/0.05;
        } else {
          next_distribution[i] = 1.0/Model::DATA_COLLECTOR->current_tf_by_therapy()[therapy_list[i]->id()];
        }
        sum += next_distribution[i];
      }

      for (std::size_t i = 0; i < distribution.size(); i++) {
        next_distribution[i] = next_distribution[i]/sum;
      }
      latest_adjust_distribution_time = Model::SCHEDULER->current_time() + delay_until_actual_trigger;
      LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date}
                << ": MFT Rebalancing will adjust distribution after " <<
                delay_until_actual_trigger << "days";
    }
  }

}

void MFTRebalancingStrategy::adjust_started_time_point(const int &current_time) {
  next_update_time = Model::SCHEDULER->current_time() + update_duration_after_rebalancing;
  latest_adjust_distribution_time = -1;
}
