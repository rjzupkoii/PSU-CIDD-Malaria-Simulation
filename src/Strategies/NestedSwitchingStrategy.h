/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NestedSwitchingStrategy.h
 * Author: Merlin
 *
 * Created on August 25, 2017, 11:07 AM
 */

#ifndef NESTEDSWITCHINGSTRATEGY_H
#define NESTEDSWITCHINGSTRATEGY_H

#include "IStrategy.h"
#include "Core/PropertyMacro.h"

class Config;

class NestedSwitchingStrategy : public IStrategy {
DISALLOW_COPY_AND_ASSIGN(NestedSwitchingStrategy)
DISALLOW_MOVE(NestedSwitchingStrategy)

public:
  std::vector<IStrategy*> strategy_list;
  std::vector<double> distribution;
  std::vector<double> start_distribution;
  std::vector<double> end_distribution;

  int strategy_switching_day;
  int switch_to_strategy_id;

  NestedSwitchingStrategy();
  //    NestedSwitchingStrategy(const NestedSwitchingStrategy& orig);
  virtual ~NestedSwitchingStrategy();

  virtual void add_strategy(IStrategy* strategy);

  void add_therapy(Therapy* therapy) override;

  Therapy* get_therapy(Person* person) override;

  std::string to_string() const override;

  StrategyType get_type() const override;

  /**
   * This function will be executed at end of time step, to check and switch therapy if needed
   */
  void update_end_of_time_step() override;

  void adjustDisttribution(int time, int totaltime);
  
  void adjust_started_time_point(const int& current_time) override;
  void monthly_update() override;
private:

};

#endif /* NESTEDSWITCHINGSTRATEGY_H */
