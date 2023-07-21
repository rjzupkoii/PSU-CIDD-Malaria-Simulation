/* 
 * CyclingStrategy.h
 *
 * Define the class for the drug cycling strategy.
 */

#ifndef CYCLINGSTRATEGY_H
#define CYCLINGSTRATEGY_H

#include "IStrategy.h"
#include "Core/PropertyMacro.h"

class CyclingStrategy : public IStrategy {
 DISALLOW_COPY_AND_ASSIGN(CyclingStrategy)

 public:
  std::vector<Therapy *> therapy_list;
  int index{0};
  int cycling_time{0};
  int next_switching_day{0};

  CyclingStrategy();

  ~CyclingStrategy() override;

  void add_therapy(Therapy *therapy) override;

  virtual void switch_therapy();

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  void update_end_of_time_step() override;

  void adjust_started_time_point(const int &current_time) override;

  void monthly_update() override;

 private:

};

#endif
