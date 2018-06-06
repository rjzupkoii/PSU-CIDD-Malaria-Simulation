/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TACTStrategy.h
 * Author: Merlin
 *
 * Created on August 23, 2017, 3:05 PM
 */

#ifndef TACTSTRATEGY_H
#define TACTSTRATEGY_H

#include "ACTIncreaseStrategy.h"

class TACTSwitchingTStrategy : public ACTIncreaseStrategy {
    DISALLOW_COPY_AND_ASSIGN_(TACTSwitchingTStrategy)
    VIRTUAL_PROPERTY_REF(int, TACT_switching_day)
    VIRTUAL_PROPERTY_REF(int, TACT_id)
public:
    TACTSwitchingTStrategy();
    //    TACTSwitchingTStrategy(const TACTSwitchingTStrategy& orig);
    virtual ~TACTSwitchingTStrategy();

    virtual std::string to_string() const;
    virtual void update_end_of_time_step();



private:

};

#endif /* TACTSTRATEGY_H */

