//
// Created by Nguyen Tran on 3/17/2018.
//

#ifndef POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
#define POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H

#include "IStrategy.h"
#include "Core/TypeDef.h"


class Config;

class NestedMFTDifferentDistributionByLocationStrategy : public IStrategy {
DISALLOW_COPY_AND_ASSIGN(NestedMFTDifferentDistributionByLocationStrategy)
DISALLOW_MOVE(NestedMFTDifferentDistributionByLocationStrategy)
public:
  std::vector<IStrategy*> strategy_list;
  DoubleVector2 distribution;
  DoubleVector2 start_distribution;
  int peak_at{0};
  int starting_time{0};

  NestedMFTDifferentDistributionByLocationStrategy();
  //    NestedSwitchingStrategy(const NestedSwitchingStrategy& orig);
  virtual ~NestedMFTDifferentDistributionByLocationStrategy();

  virtual void add_strategy(IStrategy* strategy);

  void add_therapy(Therapy* therapy) override;

  Therapy* get_therapy(Person* person) override;

  std::string to_string() const override;

  /**
   * This function will be executed at end of time step, to check and switch therapy if needed
   */
  void update_end_of_time_step() override;

  void adjust_distribution_yearly(const int &time, const int& peak_at);

  void adjust_started_time_point(const int& current_time) override;
  void monthly_update() override;

};


#endif //POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
