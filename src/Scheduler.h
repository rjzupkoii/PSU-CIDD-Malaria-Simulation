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
#include "TypeDef.h"

class Model;

class Scheduler {
DISALLOW_COPY_AND_ASSIGN(Scheduler)

DISALLOW_MOVE(Scheduler)

PROPERTY_REF(int, current_time)

PROPERTY_HEADER(int, total_time)

POINTER_PROPERTY(Model, model)

PROPERTY_REF(EventPtrVector2, timed_events_list)
PROPERTY_REF(bool, is_force_stop)

public:
  date::sys_days calendar_date;

public:
  explicit Scheduler(Model* model = nullptr);

  virtual ~Scheduler();


  void clear_all_events();

  virtual void schedule(Event* event);

  virtual void cancel(Event* event);

  void initialize(
    const date::year_month_day& starting_date, const int& total_time);

  void run();

  void begin_time_step() const;

  void end_time_step() const;

  bool can_stop() const;

  void update_end_of_time_step() const;

  void update_force_of_infection() const;

  void report_end_of_time_step() const;

  int current_day_in_year() const;

  bool is_last_day_of_month() const;

  bool is_today_monthly_reporting_day() const;

  bool is_last_day_of_year() const;

private:

  void update_treatment_coverage() const;
};

#endif  /* SCHEDULER_H */
