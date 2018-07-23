/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NovelNonACTSwitchingStrategy.h
 * Author: Merlin
 *
 * Created on August 23, 2017, 3:00 PM
 */

#ifndef NOVELNONACTSWITCHINGSTRATEGY_H
#define NOVELNONACTSWITCHINGSTRATEGY_H

#include "MFTStrategy.h"

class NovelNonACTSwitchingStrategy : public MFTStrategy {
DISALLOW_COPY_AND_ASSIGN(NovelNonACTSwitchingStrategy)
DISALLOW_MOVE(NovelNonACTSwitchingStrategy)

  int non_artemisinin_switching_day{0};
  int non_art_therapy_id{0};
  double fraction_non_art_replacement{0};
public:
  NovelNonACTSwitchingStrategy();
  //    NovelNonACTSwitchingStrategy(const NovelNonACTSwitchingStrategy& orig);
  virtual ~NovelNonACTSwitchingStrategy();

  void update_end_of_time_step() override;

  std::string to_string() const override;

};

#endif /* NOVELNONACTSWITCHINGSTRATEGY_H */
