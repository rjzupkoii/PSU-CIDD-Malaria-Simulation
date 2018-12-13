#ifndef NESTEDMFTSTRATEGY_H
#define NESTEDMFTSTRATEGY_H

#include "IStrategy.h"

class NestedMFTStrategy : public IStrategy {
 DISALLOW_COPY_AND_ASSIGN(NestedMFTStrategy)

 DISALLOW_MOVE(NestedMFTStrategy)

 public:
  std::vector<IStrategy *> strategy_list;
  std::vector<double> distribution;
  std::vector<double> start_distribution;
  std::vector<double> peak_distribution;
  int starting_time{0};
  int peak_after{0};

  NestedMFTStrategy()
      : IStrategy("NestedMFTStrategy", NestedMFT) {}

  virtual ~NestedMFTStrategy() = default;

  virtual void add_strategy(IStrategy *strategy);

  void add_therapy(Therapy *therapy) override;

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  void adjust_started_time_point(const int &current_time) override;

  void update_end_of_time_step() override;

  void monthly_update() override;

  void adjust_distribution(const int &time);
};

#endif // NESTEDMFTSTRATEGY_H
