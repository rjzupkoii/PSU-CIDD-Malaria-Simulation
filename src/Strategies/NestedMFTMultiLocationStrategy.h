//
// Created by Nguyen Tran on 3/17/2018.
//

#ifndef POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
#define POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H

#include "IStrategy.h"
#include "Core/TypeDef.h"

class Config;

class NestedMFTMultiLocationStrategy : public IStrategy {
 DISALLOW_COPY_AND_ASSIGN(NestedMFTMultiLocationStrategy)

 DISALLOW_MOVE(NestedMFTMultiLocationStrategy)

 public:
  std::vector<IStrategy *> strategy_list;
  DoubleVector2 distribution;
  DoubleVector2 start_distribution;
  DoubleVector2 peak_distribution;
  int starting_time{0};
  int peak_after{0};

  NestedMFTMultiLocationStrategy();

  //    NestedSwitchingStrategy(const NestedSwitchingStrategy& orig);
  virtual ~NestedMFTMultiLocationStrategy();

  virtual void add_strategy(IStrategy *strategy);

  void add_therapy(Therapy *therapy) override;

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  /**
  * This function will be executed at end of time step, to check and switch therapy if needed
  */
  void update_end_of_time_step() override;

  void adjust_distribution(const int &time);

  void adjust_started_time_point(const int &current_time) override;

  void monthly_update() override;

};

#endif //POMS_NESTEDSWITCHINGDIFFERENTDISTRIBUTIONBYLOCATION_H
