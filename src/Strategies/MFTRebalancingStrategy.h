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
#include "../PropertyMacro.h"

class MFTRebalancingStrategy : public MFTStrategy {
    DISALLOW_COPY_AND_ASSIGN_(MFTRebalancingStrategy)
    VIRTUAL_PROPERTY_REF(int, update_duration_after_rebalancing)

    VIRTUAL_PROPERTY_REF(int, latest_adjust_distribution_time)
    VIRTUAL_PROPERTY_REF(int, delay_until_actual_trigger)
    VIRTUAL_PROPERTY_REF(int, next_update_time)
    VIRTUAL_PROPERTY_REF(std::vector<double>, next_distribution)
    
public:
    MFTRebalancingStrategy();
    //        SmartMFTStrategy(const SmartMFTStrategy & orig);
    virtual ~MFTRebalancingStrategy();


    virtual std::string to_string() const;

    virtual IStrategy::StrategyType get_type() const;

    virtual void update_end_of_time_step();




private:

};

#endif /* SMARTMFTSTRATEGY_H */

