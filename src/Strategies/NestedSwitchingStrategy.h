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
#include "../PropertyMacro.h"

class NestedSwitchingStrategy : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(NestedSwitchingStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<IStrategy*>, strategy_list)
    VIRTUAL_PROPERTY_REF(std::vector<double>, distribution)
    VIRTUAL_PROPERTY_REF(std::vector<double>, start_distribution)
    VIRTUAL_PROPERTY_REF(std::vector<double>, end_distribution)

    VIRTUAL_PROPERTY_REF(int, strategy_switching_day)
    VIRTUAL_PROPERTY_REF(int, switch_to_strategy_id)
public:
    NestedSwitchingStrategy();
    //    NestedSwitchingStrategy(const NestedSwitchingStrategy& orig);
    virtual ~NestedSwitchingStrategy();

    virtual void add_strategy(IStrategy* strategy);

    virtual void add_therapy(Therapy* therapy);

    virtual Therapy *get_therapy(Person *person);

    virtual std::string to_string() const;

    virtual StrategyType get_type() const;

    /**
     * This function will be executed at end of time step, to check and switch therapy if needed
     */
    virtual void update_end_of_time_step();

    void adjustDisttribution(int time, int totaltime);
    
    void initialize_update_time();

private:

};

#endif /* NESTEDSWITCHINGSTRATEGY_H */

