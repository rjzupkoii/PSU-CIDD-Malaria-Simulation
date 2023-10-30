/*
 * DistrictMftStrategy.cpp
 *
 * Implement the district MFT strategy.
 *
 * NOTE that there may be a memory leak present in the district_strategies object, but since the object is going to be
 * in scope for the entire expected lifecycle of the simulation it's not a pressing concern.
 */
#include "DistrictMftStrategy.h"

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "GIS/SpatialData.h"
#include "Model.h"
#include "Population/Person.h"

DistrictMftStrategy::DistrictMftStrategy() : IStrategy("DistrictMFT", StrategyType::DistrictMftStrategy) {
  for (auto ndx = 0; ndx < SpatialData::get_instance().get_district_count(); ndx++) {
    district_strategies[ndx] = nullptr;
  }
}

void DistrictMftStrategy::add_therapy(Therapy *therapy) {
  throw std::runtime_error("Invalid function called to add therapy to the District MFT Strategy.");
}

void DistrictMftStrategy::assign_mft(int district, MftStrategy* mft) {
  district_strategies[district] = mft;
}

Therapy *DistrictMftStrategy::get_therapy(Person *person) {
  // Resolve the MFT for this district
  auto district = SpatialData::get_instance().get_district(person->location());
  auto mft = district_strategies[district];

  // Select the therapy to give the individual
  auto pr = Model::RANDOM->random_flat(0.0, 1.0);
  auto sum = 0.0;
  for (auto ndx = 0; ndx < mft->percentages.size(); ndx++) {
    sum += mft->percentages[ndx];
    if (pr <= sum) {
      return Model::CONFIG->therapy_db()[mft->therapies[ndx]];
    }
  }

  // Since we should ways return above, throw an error if we get here
  throw std::runtime_error("Scanned for therapy without finding a match: " + this->name() +
    ", district: " + std::to_string(district) + ", pr: " + std::to_string(pr));
}
