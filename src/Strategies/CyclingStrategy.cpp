/* 
 * File:   CyclingStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 4, 2013, 11:10 AM
 */

#include "CyclingStrategy.h"
#include "Model.h"
#include "Core/Scheduler.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include <sstream>
#include "IStrategy.h"
#include "Therapies/Therapy.hxx"

CyclingStrategy::CyclingStrategy() : IStrategy("CyclingStrategy", Cycling) {}

CyclingStrategy::~CyclingStrategy() = default;

void CyclingStrategy::add_therapy(Therapy *therapy) {
  therapy_list.push_back(therapy);
}

void CyclingStrategy::switch_therapy() {
  //    std::cout << "Switch from: " << index_ << "\t - to: " << index_ + 1;
  index++;
  index %= therapy_list.size();
  Model::DATA_COLLECTOR->update_UTL_vector();

  // TODO: cycling_time should be match with calendar day
  next_switching_day = Model::SCHEDULER->current_time() + cycling_time;
  LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date}
            << ": Cycling Strategy Swith Therapy to: " << therapy_list[index]->id();
}

Therapy *CyclingStrategy::get_therapy(Person *person) {

  //int index = ((Global::scheduler->currentTime - Global::startTreatmentDay) / circleTime) % therapyList.size();
  //    std::cout << therapy_list()[index_]->id() << std::endl;
  return therapy_list[index];
}

std::string CyclingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto *therapy : therapy_list) {
    sstm << sep << therapy->id();
    sep = ",";
  }
  return sstm.str();
}

void CyclingStrategy::update_end_of_time_step() {
  if (Model::SCHEDULER->current_time()==next_switching_day) {
    switch_therapy();
    //            std::cout << to_string() << std::endl;
  }
}

void CyclingStrategy::adjust_started_time_point(const int &current_time) {
  next_switching_day = Model::SCHEDULER->current_time() + cycling_time;
  index = 0;
}

void CyclingStrategy::monthly_update() {}
