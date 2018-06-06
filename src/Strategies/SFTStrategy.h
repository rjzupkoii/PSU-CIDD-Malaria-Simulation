/* 
 * File:   SFTStrategy.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 8:00 PM
 */

#ifndef SFTSTRATEGY_H
#define SFTSTRATEGY_H

#include "../PropertyMacro.h"
#include "IStrategy.h"

class SFTStrategy : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(SFTStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<Therapy*>, therapy_list)

public:
    SFTStrategy();
    //    SFTStrategy(const SFTStrategy& orig);
    virtual ~SFTStrategy();

    virtual std::vector<Therapy*>& get_therapy_list();

    virtual void add_therapy(Therapy* therapy);

    virtual bool is_strategy(const std::string& sName);

    virtual Therapy *get_therapy(Person *person);

    virtual std::string to_string() const;

    virtual IStrategy::StrategyType get_type() const;

    virtual void update_end_of_time_step();


private:

};

#endif /* SFTSTRATEGY_H */

