/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SmartMFTStrategy.h
 * Author: Merlin
 *
 * Created on August 25, 2017, 11:57 AM
 */

#ifndef SMARTMFTSTRATEGY_H
#define SMARTMFTSTRATEGY_H

#include "MFTStrategy.h"
#include "Core/PropertyMacro.h"

class MFTRebalancingStrategy : public MFTStrategy {
 DISALLOW_COPY_AND_ASSIGN(MFTRebalancingStrategy)

 DISALLOW_MOVE(MFTRebalancingStrategy)

 public:
  int update_duration_after_rebalancing{365};
  int latest_adjust_distribution_time{0};
  int delay_until_actual_trigger{365};
  int next_update_time{0};
  std::vector<double> next_distribution{0};

  MFTRebalancingStrategy();

  //        SmartMFTStrategy(const SmartMFTStrategy & orig);
  virtual ~MFTRebalancingStrategy();

  std::string to_string() const override;

  void update_end_of_time_step() override;

  void adjust_started_time_point(const int &current_time) override;
};

#endif /* SMARTMFTSTRATEGY_H */
