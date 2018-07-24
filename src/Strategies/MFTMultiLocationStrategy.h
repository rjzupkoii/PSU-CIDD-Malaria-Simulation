//
// Created by Nguyen Tran on 3/16/2018.
//

#ifndef POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H
#define POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H


#include "IStrategy.h"
#include "Core/TypeDef.h"

class MFTMultiLocationStrategy : public IStrategy {
DISALLOW_COPY_AND_ASSIGN(MFTMultiLocationStrategy)
DISALLOW_MOVE(MFTMultiLocationStrategy)

public:
  std::vector<Therapy *> therapy_list;
  DoubleVector2 distribution_by_location;

  MFTMultiLocationStrategy();
  //    MFTStrategy(const MFTStrategy& orig);
  virtual ~MFTMultiLocationStrategy();
  void add_therapy(Therapy* therapy) override;
  Therapy* get_therapy(Person* person) override;
  std::string to_string() const override;
  void update_end_of_time_step() override;
  void adjust_started_time_point(const int& current_time) override;
  void monthly_update() override;
};


#endif //POMS_MFTDIFFERENTDISTRIBUTIONBYLOCATIONSTRATEGY_H
