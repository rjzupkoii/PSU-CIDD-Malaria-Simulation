//
// Created by Nguyen Tran on 3/17/2018.
//

#ifndef POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
#define POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H

#include "IStrategy.h"
#include "../TypeDef.h"


class NestedSwitchingDifferentDistributionByLocationStrategy  : public IStrategy {
    DISALLOW_COPY_AND_ASSIGN_(NestedSwitchingDifferentDistributionByLocationStrategy)
    VIRTUAL_PROPERTY_REF(std::vector<IStrategy*>, strategy_list)
    VIRTUAL_PROPERTY_REF(DoubleVector2, distribution)
    VIRTUAL_PROPERTY_REF(DoubleVector2, start_distribution)

    VIRTUAL_PROPERTY_REF(int, strategy_switching_day)
    VIRTUAL_PROPERTY_REF(int, switch_to_strategy_id)
    VIRTUAL_PROPERTY_REF(int, peak_at)
public:
    NestedSwitchingDifferentDistributionByLocationStrategy();
    //    NestedSwitchingStrategy(const NestedSwitchingStrategy& orig);
    virtual ~NestedSwitchingDifferentDistributionByLocationStrategy();

    virtual void add_strategy(IStrategy* strategy);

    void add_therapy(Therapy* therapy) override;

    Therapy *get_therapy(Person *person) override;

    std::string to_string() const override;

    StrategyType get_type() const override;

    /**
     * This function will be executed at end of time step, to check and switch therapy if needed
     */
    void update_end_of_time_step() override;

    void adjustDistribution(int time, int totaltime);

    void initialize_update_time();

private:

};


#endif //POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
