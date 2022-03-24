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

 // Number of days to wait between updating the user
 PROPERTY(int, days_between_notifications)

 public:
  date::sys_days calendar_date;

  EventPtrVector2 individual_events_list_;
  EventPtrVector2 population_events_list_;

  explicit Scheduler(Model *model = nullptr);

  virtual ~Scheduler();

  void extend_total_time(int new_total_time);

  void clear_all_events();

  void clear_all_events(EventPtrVector2 &events_list) const;

  virtual void schedule_individual_event(Event *event);

  // TODO Update references for "population" to "global" since it better reflects the type of event
  virtual void schedule_population_event(Event *event);

  virtual void schedule_event(EventPtrVector &time_events, Event *event);

  virtual void cancel(Event *event);

  void execute_events_list(EventPtrVector &events_list) const;

  void initialize(const date::year_month_day &starting_date, const int &total_time);

  void run();

  void begin_time_step() const;

  void end_time_step() const;

  bool can_stop() const;

  int current_day_in_year() const;

  bool is_today_last_day_of_month() const;

  bool is_today_first_day_of_month() const;

  bool is_today_first_day_of_year() const;

  bool is_today_last_day_of_year() const;

};

#endif  /* SCHEDULER_H */
