/* 
 * NonInfantImmuneComponent.cpp
 *
 * Define the immune component for individuals that are not infants.
 */
#include "NonInfantImmuneComponent.h"

#include "Core/Config/Config.h"
#include "Model.h"

NonInfantImmuneComponent::NonInfantImmuneComponent(ImmuneSystem *immune_system) : ImmuneComponent(immune_system) { }

double NonInfantImmuneComponent::get_acquire_rate(const int &age) const {
  return (age > 80) ? Model::CONFIG->immune_system_information().acquire_rate_by_age[80]
                    : Model::CONFIG->immune_system_information().acquire_rate_by_age[age];
}

double NonInfantImmuneComponent::get_decay_rate(const int &age) const {
  return Model::CONFIG->immune_system_information().decay_rate;
}