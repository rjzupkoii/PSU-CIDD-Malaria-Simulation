/* 
 * ImmuneSystem.cpp
 *
 * Implement the immune system for the individuals.
 */
#include "ImmuneSystem.h"

#include <cmath>

#include "Core/Config/Config.h"
#include "Helpers/ObjectHelpers.h"
#include "Model.h"
#include "Person.h"
#include "Population/ImmuneComponent/ImmuneComponent.h"

OBJECTPOOL_IMPL(ImmuneSystem)

ImmuneSystem::ImmuneSystem(Person *p) : person_(p), increase_(false), immune_component_(nullptr) { }

ImmuneSystem::~ImmuneSystem() {

  if (immune_component_!=nullptr) {
    ObjectHelpers::delete_pointer<ImmuneComponent>(immune_component_);
  }
  assert(immune_component_==nullptr);
  person_ = nullptr;
}

ImmuneComponent *ImmuneSystem::immune_component() const {
  return immune_component_;
}

void ImmuneSystem::set_immune_component(ImmuneComponent *value) {
  if (immune_component_!=value) {
    if (immune_component_!=nullptr) {
      ObjectHelpers::delete_pointer<ImmuneComponent>(immune_component_);
    }

    immune_component_ = value;
    immune_component_->set_immune_system(this);
  }
}

double ImmuneSystem::get_latest_immune_value() const {
  return immune_component_->latest_value();
}

void ImmuneSystem::set_latest_immune_value(double value) {
  immune_component_->set_latest_value(value);
}

double ImmuneSystem::get_current_value() const {
  return immune_component_->get_current_value();
}

double ImmuneSystem::get_parasite_size_after_t_days(const int &duration, const double &originalSize,
                                                    const double &fitness) const {

  const auto last_immune_level = get_latest_immune_value();
  const auto temp = Model::CONFIG->immune_system_information().c_max*(1 - last_immune_level) + Model::CONFIG
      ->immune_system_information()
      .c_min*
      last_immune_level;

  const auto value = originalSize + duration*(log10(temp) + log10(fitness));
  return value;

}

double ImmuneSystem::get_clinical_progression_probability() const {

  const auto immune = get_current_value();
  const auto isf = Model::CONFIG->immune_system_information();

  const auto pr_clinical = isf.max_clinical_probability / 
    (1 + pow((immune / isf.midpoint), isf.immune_effect_on_progression_to_clinical));

  return pr_clinical;
}

void ImmuneSystem::update() {
  immune_component_->update();
}
