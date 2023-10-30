/*
 * IStrategy.h
 *
 * Define the abstract base class for drug policy strategies along with associated enumeration and helper map.
 */
#ifndef ISTRATEGY_H
#define ISTRATEGY_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Core/PropertyMacro.h"

class Therapy;
class Person;

class IStrategy {
  DISALLOW_COPY_AND_ASSIGN(IStrategy)

public:
  // Define the various types, note that the numbers are odd since legacy code or results may depend on them
  enum StrategyType {
    SFT = 0,
    Cycling = 1,
    MFT = 2,
    MFTMultiLocation = 5,
    NestedMFT = 6,
    NestedMFTMultiLocation = 7,
    DistrictMftStrategy = 8
  };

  static inline std::map<std::string, IStrategy::StrategyType> StrategyTypeMap {
          {"SFT", SFT},
          {"Cycling", Cycling},
          {"MFT", MFT},
          {"NestedMFT", NestedMFT},
          {"MFTMultiLocation", MFTMultiLocation},
          {"NestedMFTMultiLocation", NestedMFTMultiLocation},
          {"DistrictMFT", DistrictMftStrategy}
  };

  PROPERTY(int, id)
  PROPERTY(std::string, name)
  PROPERTY(StrategyType, type)

  IStrategy(std::string name, const StrategyType &type) : id_(-1), name_{ std::move(name) }, type_{ type } { }

  virtual ~IStrategy() = default;

  virtual void add_therapy(Therapy *therapy) = 0;

  virtual Therapy *get_therapy(Person *person) = 0;

  [[nodiscard]] virtual std::string to_string() const = 0;

  virtual void adjust_started_time_point(const int &current_time) = 0;

  // This function will be executed at end of time step, to check and switch therapy if needed
  virtual void update_end_of_time_step() = 0;

  virtual void monthly_update() = 0;

};

#endif
