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
    DISALLOW_COPY_AND_ASSIGN_(NovelNonACTSwitchingStrategy)
    VIRTUAL_PROPERTY_REF(int, non_artemisinin_switching_day)
    VIRTUAL_PROPERTY_REF(int, non_art_therapy_id)
    VIRTUAL_PROPERTY_REF(double, fraction_non_art_replacement)
public:
    NovelNonACTSwitchingStrategy();
    //    NovelNonACTSwitchingStrategy(const NovelNonACTSwitchingStrategy& orig);
    virtual ~NovelNonACTSwitchingStrategy();

    virtual IStrategy::StrategyType get_type() const;

    virtual std::string to_string() const;

    virtual void update_end_of_time_step();

private:

};

#endif /* NOVELNONACTSWITCHINGSTRATEGY_H */

