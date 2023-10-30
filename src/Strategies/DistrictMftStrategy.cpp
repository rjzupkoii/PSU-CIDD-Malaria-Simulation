/*
 * DistrictMftStrategy.cpp
 *
 * Implement the district MFT strategy.
 */
#include "DistrictMftStrategy.h"

#include "Core/Random.h"
#include "GIS/SpatialData.h"
#include "Model.h"
#include "Population/Person.h"

DistrictMftStrategy::DistrictMftStrategy() : IStrategy("DistrictMFT", StrategyType::DistrictMftStrategy) {
  auto districts = SpatialData::get_instance().get_district_count();
  district_strategies = std::vector<MftStrategy>(districts);
}

void DistrictMftStrategy::add_therapy(Therapy *therapy) {
  throw std::runtime_error("Invalid function called to add therapy to the District MFT Strategy.");
}

void DistrictMftStrategy::add_therapy(Therapy *therapy, int district, float percentage) {
  district_strategies[district].therapies.push_back(therapy);
  district_strategies[district].percentages.push_back(percentage);
}

Therapy *DistrictMftStrategy::get_therapy(Person *person) {
  // Resolve the MFT for this district
  auto district = SpatialData::get_instance().get_district(person->location());
  auto mft = district_strategies[district];

  // Select the therapy to give the individual
  auto pr = Model::RANDOM->random_flat(0.0, 1.0);
  auto sum = 0.0;
  for (auto ndx = 0; ndx < mft.percentages.size(); ndx++) {
    sum += mft.percentages[ndx];
    if (pr <= sum) {
      return mft.therapies[ndx];
    }
  }

  // Since we should ways return above, throw an error if we get here
  throw std::runtime_error("Scanned for therapy without finding a match: " + this->name() +
    ", district: " + std::to_string(district) + ", pr: " + std::to_string(pr));
}

std::string DistrictMftStrategy::validate(DistrictMftStrategy strategy) {
  // Define our variables
  std::string errors;

  // Iterate over all the strategies to confirm that they are valid
  for (auto id = 0; id < SpatialData::get_instance().get_district_count(); id++) {
    auto mft = strategy.district_strategies[id];

    // There should be at least one therapy defined
    if (mft.therapies.empty()) {
      errors.append("No therapies defined for district id: " + std::to_string(id));
    }

    // Sum of percentages should be 100%
    auto total = 0.0;
    for (auto value : mft.percentages) {
      total += value;
    }
    if (int(total) != 1) {
      errors.append("Total of the percentages is not 100% for district id: " + std::to_string(id));
    }
  }

  // Return the errors that were found
  return errors;
}
