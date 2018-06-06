//
// Created by Nguyen Tran on 3/16/2018.
//

#ifndef POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H
#define POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H


#include "IStrategy.h"
#include "../TypeDef.h"


class MFTDifferentDistributionByLocationStrategy : public IStrategy {
DISALLOW_COPY_AND_ASSIGN_(MFTDifferentDistributionByLocationStrategy)

VIRTUAL_PROPERTY_REF(std::vector<Therapy *>, therapy_list)

VIRTUAL_PROPERTY_REF(DoubleVector2, distribution_by_location)


public:
    MFTDifferentDistributionByLocationStrategy();

    //    MFTStrategy(const MFTStrategy& orig);
    virtual ~MFTDifferentDistributionByLocationStrategy();

    void add_therapy(Therapy *therapy) override;

    Therapy *get_therapy(Person *person) override;

    std::string to_string() const override;

    IStrategy::StrategyType get_type() const override;

    void update_end_of_time_step() override;

private:

};


#endif //POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H
