/* 
 * File:   Scheduler.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:27 PM
 */

#include <iomanip>
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

Scheduler::Scheduler(Model *model) : current_time_(-1), total_available_time_(-1), model_(model),
                                     is_force_stop_(false) {}

Scheduler::~Scheduler() {
  clear_all_events();
}

void Scheduler::extend_total_time(int new_total_time) {
  if (total_available_time_ < new_total_time)
    for (auto i = total_available_time_; i <= new_total_time; i++) {
      individual_events_list_.push_back(EventPtrVector());
      population_events_list_.push_back(EventPtrVector());
    }
  total_available_time_ = new_total_time;
}

void Scheduler::clear_all_events() {
  clear_all_events(individual_events_list_);
  clear_all_events(population_events_list_);
}

void Scheduler::initialize(const date::year_month_day &starting_date, const int &total_time) {
  //TODO: not urgent, why +1
  set_total_available_time(total_time + 1);
  set_current_time(0);

  calendar_date = sys_days(starting_date);
}

void Scheduler::clear_all_events(EventPtrVector2 &events_list) const {
  for (auto &timestep_events : events_list) {
    for (auto *event : timestep_events) {
      if (event->dispatcher!=nullptr) {
        event->dispatcher->remove(event);
      }
      ObjectHelpers::delete_pointer<Event>(event);
    }
    timestep_events.clear();
  }
  events_list.clear();
}

int Scheduler::total_available_time() const {
  return total_available_time_;
}

void Scheduler::set_total_available_time(const int &value) {
  if (total_available_time_ > 0) {
    clear_all_events();
  }
  total_available_time_ = value;
  individual_events_list_.assign(total_available_time_, EventPtrVector());
  population_events_list_.assign(total_available_time_, EventPtrVector());
}

void Scheduler::schedule_individual_event(Event *event) {
  schedule_event(individual_events_list_[event->time], event);
}

void Scheduler::schedule_population_event(Event *event) {
  schedule_event(population_events_list_[event->time], event);
}

void Scheduler::schedule_event(EventPtrVector &time_events, Event *event) {
  // Schedule event in the future
  // Event time cannot exceed total time or less than current time
  if (event->time > Model::CONFIG->total_time() || event->time < current_time_) {
    LOG_IF(event->time < current_time_, FATAL) << "Error when scheduling event " << event->name() << " at "
                                               << event->time
                                               << ". Current_time: " << current_time_ << " - total time: "
                                               << total_available_time_;
    VLOG(2) << "Cannot schedule event " << event->name() << " at " << event->time << ". Current_time: "
            << current_time_ << " - total time: " << total_available_time_;
    ObjectHelpers::delete_pointer<Event>(event);
  } else {
    time_events.push_back(event);
    event->scheduler = this;
    event->executable = true;
  }
}

void Scheduler::cancel(Event *event) {
  event->executable = false;
}

void Scheduler::execute_events_list(EventPtrVector &events_list) const {
  for (auto &event : events_list) {
    // std::cout << event->name() << std::endl;
    event->perform_execute();
    ObjectHelpers::delete_pointer<Event>(event);
  }
  ObjectHelpers::clear_vector_memory<Event>(events_list);
}

void Scheduler::run() {

  // Make sure we have a model
  if (model_ == nullptr) {
    throw std::runtime_error("Scheduler::run() called without model!");
  }

  LOG(INFO) << "Simulation is running";
  current_time_ = 0;

  for (current_time_ = 0; !can_stop(); current_time_++) {
    std::time_t t = std::time(nullptr);
    LOG_IF(current_time_ % days_between_notifications_ == 0, INFO) <<  std::put_time(std::localtime(&t), "%H:%M:%S - ") <<  "Day: " << current_time_;

    begin_time_step();
    // population related events
    execute_events_list(population_events_list_[current_time_]);
    // population related events
    model_->perform_population_events_daily();
    // individual related events
    execute_events_list(individual_events_list_[current_time_]);

    end_time_step();
    calendar_date += days{1};
  }
}

void Scheduler::begin_time_step() const {
  model_->begin_time_step();
  if (is_today_first_day_of_month()) {
    model_->monthly_update();
  }

  if (is_today_first_day_of_year()) {
    // std::cout << date::year_month_day{calendar_date} << std::endl;
    model_->yearly_update();
  }
}

void Scheduler::end_time_step() const {
  model_->daily_update(current_time_);
}

bool Scheduler::can_stop() const {
  return current_time_ > Model::CONFIG->total_time() || is_force_stop_;
}

int Scheduler::current_day_in_year() const {
  return TimeHelpers::day_of_year(calendar_date);
}

bool Scheduler::is_today_last_day_of_year() const {
  year_month_day ymd{calendar_date};
  return ymd.month()==month{12} && ymd.day()==day{31};
}

bool Scheduler::is_today_first_day_of_month() const {
  // return true;
  year_month_day ymd{calendar_date};
  return ymd.day()==day{1};
}

bool Scheduler::is_today_first_day_of_year() const {
  year_month_day ymd{calendar_date};
  return ymd.month()==month{1} && ymd.day()==day{1};
}

bool Scheduler::is_today_last_day_of_month() const {
  const auto next_date = calendar_date + days{1};
  year_month_day ymd{next_date};
  return ymd.day()==day{1};
}
