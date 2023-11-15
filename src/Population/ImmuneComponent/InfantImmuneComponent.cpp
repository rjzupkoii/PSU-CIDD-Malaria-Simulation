/* 
 * InfantImmuneComponent.cpp
 *
 * Implement the immune component for individuals that are infants.
 */
#include "InfantImmuneComponent.h"

#include <cmath>

#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/Person.h"

InfantImmuneComponent::InfantImmuneComponent(ImmuneSystem *immune_system) : ImmuneComponent(immune_system) { }

double InfantImmuneComponent::get_acquire_rate(const int &age) const {
  return 0;
}

double InfantImmuneComponent::get_decay_rate(const int &age) const {
  return 0.0315;
}

double InfantImmuneComponent::get_current_value() {
  auto temp = 0.0;
  if (immune_system() != nullptr && immune_system()->person() != nullptr) {
    const auto current_time = Model::SCHEDULER->current_time();
    const auto duration = current_time - immune_system()->person()->latest_update_time();
    // Decrease immune response by: I(t) = I0 * e ^ (-b2*t);
    temp = latest_value()*exp(-get_decay_rate(0)*duration);
  }
  return temp;
}
