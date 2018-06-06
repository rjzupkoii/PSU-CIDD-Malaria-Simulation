/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IStrategy.h
 * Author: Merlin
 *
 * Created on August 21, 2017, 3:45 PM
 */

#ifndef ISTRATEGY_H
#define ISTRATEGY_H
#include <string>
#include <vector>
#include "../PropertyMacro.h"
#include <map>

class Therapy;
class Person;
class IStrategy {
public:

    enum StrategyType {
        SFT = 0,
        Cycling = 1,
        MFT = 2,
        AdaptiveCycling = 3,
        ACTIncreaseOvertime = 4,
        NovelNonACTSwitching = 5,
        TACTSwitching = 6,
        MFTRebalancing = 7,
        NestedSwitching = 8,
        MFTDifferentDistributionByLocation = 9,
        NestedSwitchingDifferentDistributionByLocation = 10
    };
    static std::map<std::string, StrategyType> StrategyTypeMap;

    DISALLOW_COPY_AND_ASSIGN_(IStrategy)
public:
    int id;
    std::string name;

public:

    IStrategy() {
    }

    virtual ~IStrategy() {
    }

    virtual void add_therapy(Therapy* therapy) = 0;

    virtual Therapy *get_therapy(Person *person) = 0;

    virtual std::string to_string() const = 0;

    virtual StrategyType get_type() const = 0;

    /**
     * This function will be executed at end of time step, to check and switch therapy if needed
     */
    virtual void update_end_of_time_step() = 0;

};



#endif /* ISTRATEGY_H */

