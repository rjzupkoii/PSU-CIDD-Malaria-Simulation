/*
 * DistrictMftStrategy.h
 *
 * Define the district MFT strategy class.
 */
#ifndef POMS_DISTRICTMFTSTRATEGY_H
#define POMS_DISTRICTMFTSTRATEGY_H

#include "Core/PropertyMacro.h"
#include "IStrategy.h"

class DistrictMftStrategy : public IStrategy {
  DISALLOW_COPY_AND_ASSIGN(DistrictMftStrategy)
  DISALLOW_MOVE(DistrictMftStrategy)

  struct MftStrategy {
      std::vector<Therapy*> therapies;
      std::vector<float> percentages;
  };

private:
  std::vector<MftStrategy> district_strategies;

public:
  DistrictMftStrategy();
  ~DistrictMftStrategy() override = default;

  // Override the method for IStrategy and throw an error if called.
  void add_therapy(Therapy *therapy) override;

  // Associate the given therapy with the indicated district and distribution percentage.
  void add_therapy(Therapy *therapy, int district, float percentage);

  // Get the therapy that should be given to the individual.
  Therapy *get_therapy(Person *person) override;

  // Return a string with the name of the district MFT strategy from the configuration
  [[nodiscard]] std::string to_string() const override { return this->name(); }

  // Return a string that contains the errors in the given strategy, or an empty string if there are no errors.
  static std::string validate(DistrictMftStrategy strategy);

  void adjust_started_time_point(const int &current_time) override { }
  void monthly_update() override { }
  void update_end_of_time_step() override { }
};

#endif
