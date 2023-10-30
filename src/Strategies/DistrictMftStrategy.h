/*
 * DistrictMftStrategy.h
 *
 * Define the district multiple first-line therapy (MFT) strategy class.
 */
#ifndef POMS_DISTRICTMFTSTRATEGY_H
#define POMS_DISTRICTMFTSTRATEGY_H

#include "Core/PropertyMacro.h"
#include "IStrategy.h"

class DistrictMftStrategy : public IStrategy {
  DISALLOW_COPY_AND_ASSIGN(DistrictMftStrategy)
  DISALLOW_MOVE(DistrictMftStrategy)

public:
  // The basic structure of an MFT
  struct MftStrategy {
      std::vector<int> therapies;
      std::vector<float> percentages;
  };

private:
  std::vector<MftStrategy> district_strategies;

public:
  DistrictMftStrategy();
  ~DistrictMftStrategy() override = default;

  // Override the method for IStrategy and throw an error if called.
  void add_therapy(Therapy *therapy) override;

  // Associate the given MFT with the indicated district
  void assign_mft(int district, MftStrategy mft);

  // Get the therapy that should be given to the individual.
  Therapy *get_therapy(Person *person) override;

  // Return a string with the name of the district MFT strategy from the configuration
  [[nodiscard]] std::string to_string() const override { return this->name(); }

  void adjust_started_time_point(const int &current_time) override { }
  void monthly_update() override { }
  void update_end_of_time_step() override { }
};

#endif
