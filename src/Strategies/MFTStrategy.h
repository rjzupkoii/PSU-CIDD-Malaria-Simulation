/* 
 * File:   MFTStrategy.h
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:09 AM
 */

#ifndef MFTSTRATEGY_H
#define MFTSTRATEGY_H

#include "IStrategy.h"
#include "../PropertyMacro.h"

class Random;
class Therapy;

class MFTStrategy : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(MFTStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<Therapy*>, therapy_list)
    VIRTUAL_PROPERTY_REF(std::vector<double>, distribution)

public:
    MFTStrategy();
    //    MFTStrategy(const MFTStrategy& orig);
    virtual ~MFTStrategy();
    
    virtual void add_therapy(Therapy* therapy);

    virtual Therapy *get_therapy(Person *person);

    virtual std::string to_string() const;

    virtual IStrategy::StrategyType get_type() const;

    virtual void update_end_of_time_step();

private:

};

#endif /* MFTSTRATEGY_H */

