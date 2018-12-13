/* 
 * File:   AdaptiveCyclingStrategy.h
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:10 AM
 */

#ifndef ADAPTIVECYCLINGSTRATEGY_H
#define ADAPTIVECYCLINGSTRATEGY_H

#include <vector>
#include "IStrategy.h"
#include "Core/PropertyMacro.h"

class AdaptiveCyclingStrategy : public IStrategy {
 DISALLOW_COPY_AND_ASSIGN(AdaptiveCyclingStrategy)

 DISALLOW_MOVE(AdaptiveCyclingStrategy)

 public:
  std::vector<Therapy *> therapy_list;
  int index{0};
  double trigger_value{0.1};
  int delay_until_actual_trigger{365};
  int turn_off_days{365};
  int latest_switch_time{0};

  AdaptiveCyclingStrategy();

  virtual ~AdaptiveCyclingStrategy();

  void add_therapy(Therapy *therapy) override;

  virtual void switch_therapy();

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  void update_end_of_time_step() override;

  void adjust_started_time_point(const int &current_time) override;

  void monthly_update() override;
};

#endif /* ADAPTIVECYCLINGSTRATEGY_H */
