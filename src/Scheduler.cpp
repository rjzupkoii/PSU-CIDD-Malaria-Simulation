/* 
 * File:   Scheduler.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:27 PM
 */

#include <vector>
#include "Scheduler.h"
#include "Events/Event.h"
#include "Dispatcher.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Helpers/TimeHelpers.h"
#include "Helpers/ObjectHelpers.h"
#include "easylogging++.h"

using namespace date;

Scheduler::Scheduler(Model* model) : current_time_(-1), total_time_(-1), model_(model), is_force_stop_(false) {}

Scheduler::~Scheduler() {
  clear_all_events();
}

void Scheduler::initialize(const date::year_month_day& starting_date, const int& total_time) {
  set_total_time(total_time);
  set_current_time(0);

  calendar_date = sys_days(starting_date);
}

void Scheduler::clear_all_events() {
  for (auto& events_list : timed_events_list_) {
    for (auto* event : events_list) {
      if (event->dispatcher() != nullptr) {
        event->dispatcher()->remove(event);
      }
      ObjectHelpers::delete_pointer<Event>(event);
    }
    events_list.clear();
  }
  timed_events_list_.clear();
}

int Scheduler::total_time() const {
  return total_time_;
}

void Scheduler::set_total_time(const int& value) {
  if (total_time_ > 0) {
    clear_all_events();
  }

  total_time_ = value;
  timed_events_list_.assign(total_time_, EventPtrVector());
}

void Scheduler::schedule(Event* event) {
  // Schedule event in the future
  // Event time cannot exceed total time or less than current time
  if (event->time() > total_time_ || event->time() < current_time_) {
    LOG_IF(event->time() < current_time_, FATAL) << "Error when schedule event " << event->name() << " at " << event->time()
    << ". Current_time: " << current_time_ << " - total time: " << total_time_;
    LOG(WARNING) << "Cannot schedule event " << event->name() << " at " << event->time() << ". Current_time: "
      << current_time_ << " - total time: " << total_time_;
    ObjectHelpers::delete_pointer<Event>(event);
  }
  else {
    timed_events_list_[event->time()].push_back(event);
    event->set_scheduler(this);
  }
}

void Scheduler::cancel(Event* event) {
  event->set_executable(false);
}

template <typename T>
void clear_vector_memory(std::vector<T*>& vector) {
  vector.clear();
  std::vector<T*> temp;

  vector.swap(temp);
}

void Scheduler::run() {
  LOG(INFO) << "Simulation is running";
  current_time_ = 0;


  for (current_time_ = 0; !can_stop(); current_time_++) {
    begin_time_step();
    // population related events       
    model_->perform_population_events_daily();

    // individual related events
    for (auto& event : timed_events_list_[current_time_]) {
      event->perform_execute();
      ObjectHelpers::delete_pointer<Event>(event);
    }
    clear_vector_memory<Event>(timed_events_list_[current_time_]);
    
    LOG_IF(current_time_ % 100 == 0, INFO) << "Day: " << current_time_;
       

    end_time_step();

    calendar_date += days{1};
  }
}

void Scheduler::begin_time_step() const {
  if (model_ != nullptr) {
    model_->begin_time_step();
  }
}


void Scheduler::end_time_step() const {
  if (model_ != nullptr) {

    model_->daily_update(current_time_);
    //TODO:: change to first day???
    if (is_today_last_day_of_month()) {
      model_->monthly_update();
    }

    if (is_today_last_day_of_year()) {
      model_->yearly_update();
    }
  }
}

bool Scheduler::can_stop() const {
  return current_time_ > Model::CONFIG->total_time() || is_force_stop_;
}

int Scheduler::current_day_in_year() const {
  return TimeHelpers::day_of_year(calendar_date);
}

bool Scheduler::is_today_last_day_of_year() const {
  year_month_day ymd{calendar_date};
  return (ymd.month() - January).count() == 12 && (ymd.day() - 0_d).count() == 31;
}

bool Scheduler::is_today_first_day_of_month() const {
  // return true;
  year_month_day ymd{calendar_date};
  return (ymd.day() - 0_d).count() == 1;
}

bool Scheduler::is_today_last_day_of_month() const {
  const auto next_date = calendar_date + days{1};
  year_month_day ymd{next_date};
  return (ymd.day() - 0_d).count() == 1;
}
