/* 
 * File:   Scheduler.h
 * Author: nguyentran
 *
 * Created on March 22, 2013, 2:27 PM
 */

#ifndef SCHEDULER_H
#define  SCHEDULER_H

#include <chrono>
#include "date/date.h"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class Model;

class Scheduler {
DISALLOW_COPY_AND_ASSIGN(Scheduler)

DISALLOW_MOVE(Scheduler)

PROPERTY_REF(int, current_time)

PROPERTY_HEADER(int, total_available_time)

POINTER_PROPERTY(Model, model)

PROPERTY_REF(bool, is_force_stop)

public:
  date::sys_days calendar_date;

  explicit Scheduler(Model* model = nullptr);

  virtual ~Scheduler();

  void clear_all_events();
  void clear_all_events(EventPtrVector2& events_list) const;

  virtual void schedule_individual_event(Event* event);

  virtual void schedule_population_event(Event* event);
  virtual void schedule_event(EventPtrVector& time_events, Event* event);
  

  virtual void cancel(Event* event);

  void initialize(
    const date::year_month_day& starting_date, const int& total_time);

  void run();

  void begin_time_step() const;

  void end_time_step() const;

  bool can_stop() const;

  int current_day_in_year() const;

  bool is_today_last_day_of_month() const;

  bool is_today_first_day_of_month() const;

  bool is_today_last_day_of_year() const;
private:
  EventPtrVector2 individual_events_list_;
  EventPtrVector2 population_events_list_;
};

#endif  /* SCHEDULER_H */
