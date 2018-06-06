/* 
 * File:   CyclingStrategy.h
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:10 AM
 */

#ifndef CYCLINGSTRATEGY_H
#define CYCLINGSTRATEGY_H

#include "IStrategy.h"
#include "../PropertyMacro.h"

class CyclingStrategy : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(CyclingStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<Therapy*>, therapy_list)
    VIRTUAL_PROPERTY_REF(int, index)
    VIRTUAL_PROPERTY_REF(int, cycling_time)
    VIRTUAL_PROPERTY_REF(int, next_switching_day)

public:
    CyclingStrategy();
    //    CyclingStrategy(const CyclingStrategy& orig);
    virtual ~CyclingStrategy();

    virtual void add_therapy(Therapy* therapy);

    virtual void switch_therapy();

    virtual Therapy *get_therapy(Person *person);

    virtual std::string to_string() const;

    virtual IStrategy::StrategyType get_type() const;

    virtual void update_end_of_time_step();
private:

};

#endif /* CYCLINGSTRATEGY_H */

