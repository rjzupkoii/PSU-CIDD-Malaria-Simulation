/* 
 * CyclingStrategy.cpp
 *
 * Implement the class for the drug cycling strategy.
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
  index++;
  index %= static_cast<int>(therapy_list.size());

  next_switching_day = Model::SCHEDULER->current_time() + cycling_time;
  LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date}
            << ": Cycling Strategy Switch Therapy to: " << therapy_list[index]->id();
}

Therapy *CyclingStrategy::get_therapy(Person *person) {
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
  }
}

void CyclingStrategy::adjust_started_time_point(const int &current_time) {
  next_switching_day = Model::SCHEDULER->current_time() + cycling_time;
  index = 0;
}

void CyclingStrategy::monthly_update() {}
