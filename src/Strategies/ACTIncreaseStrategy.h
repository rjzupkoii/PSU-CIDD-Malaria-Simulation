/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ACTIncreaseStrategy.h
 * Author: Merlin
 *
 * Created on April 26, 2017, 11:20 AM
 */

#ifndef ACTINCREASESTRATEGY_H
#define ACTINCREASESTRATEGY_H


#include "IStrategy.h"
#include "Core/PropertyMacro.h"

class Random;

class ACTIncreaseStrategy : public IStrategy {
DISALLOW_COPY_AND_ASSIGN(ACTIncreaseStrategy)

public:

  std::vector<Therapy*> therapy_list;
  std::vector<double> distribution;
  std::vector<double> start_distribution;
  std::vector<double> end_distribution;
  ACTIncreaseStrategy();
  //    MFTStrategy(const MFTStrategy& orig);
  virtual ~ACTIncreaseStrategy();
  void add_therapy(Therapy* therapy) override;
  Therapy* get_therapy(Person* person) override;
  void update_end_of_time_step() override;

  void adjust_disttribution(const int time, const int totaltime);

  std::string to_string() const override;
  void adjust_started_time_point(const int& current_time) override;
  void monthly_update() override;
private:

};

#endif /* ACTINCREASESTRATEGY_H */
