/* 
 * File:   AdaptiveCyclingStrategy.h
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:10 AM
 */

#ifndef ADAPTIVECYCLINGSTRATEGY_H
#define ADAPTIVECYCLINGSTRATEGY_H

#include "IStrategy.h"
#include "../PropertyMacro.h"

class AdaptiveCyclingStrategy : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(AdaptiveCyclingStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<Therapy*>, therapy_list)
    VIRTUAL_PROPERTY_REF(int, index)

    VIRTUAL_PROPERTY_REF(double, trigger_value)
    VIRTUAL_PROPERTY_REF(int, delay_until_actual_trigger)
    VIRTUAL_PROPERTY_REF(int, turn_off_days)
    VIRTUAL_PROPERTY_REF(int, latest_switch_time)


public:
    AdaptiveCyclingStrategy();
    //    AdaptiveCyclingStrategy(const AdaptiveCyclingStrategy& orig);
    virtual ~AdaptiveCyclingStrategy();
    
    virtual std::vector<Therapy*>& get_therapy_list();
    
    virtual void add_therapy(Therapy* therapy);

    virtual void switch_therapy();

    virtual Therapy *get_therapy(Person *person);

    virtual std::string to_string() const;

    virtual IStrategy::StrategyType get_type() const;

    virtual void update_end_of_time_step();

private:

};

#endif /* ADAPTIVECYCLINGSTRATEGY_H */

