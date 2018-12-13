#ifndef NOVELDRUGSWITCHINGSTRATEGY_H
#define NOVELDRUGSWITCHINGSTRATEGY_H

#include <vector>
#include "IStrategy.h"
#include "Core/PropertyMacro.h"

class NovelDrugSwitchingStrategy : public IStrategy {
 DISALLOW_COPY_AND_ASSIGN(NovelDrugSwitchingStrategy)

 DISALLOW_MOVE(NovelDrugSwitchingStrategy)

 public:
  std::vector<Therapy *> therapy_list;
  std::vector<double> distribution;

  int switch_to{0};
  double tf_threshold{0.1};

  NovelDrugSwitchingStrategy();

  virtual ~NovelDrugSwitchingStrategy() = default;

  void add_therapy(Therapy *therapy) override;

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  void update_end_of_time_step() override;

  void adjust_started_time_point(const int &current_time) override;

  void monthly_update() override;
};

#endif // NOVELDRUGSWITCHINGSTRATEGY_H
