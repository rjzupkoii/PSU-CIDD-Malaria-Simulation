/* 
 * File:   ImmuneSystem.cpp
 * Author: nguyentran
 * 
 * Created on May 27, 2013, 11:56 AM
 */

#include "ImmuneSystem.h"
#include "ImmuneComponent.h"
#include "Person.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include <cmath>
#include "Helpers/ObjectHelpers.h"

OBJECTPOOL_IMPL(ImmuneSystem)

ImmuneSystem::ImmuneSystem(Person *p) : person_(p), increase_(false), immune_component_(nullptr) {
  //    immune_components_ = new ImmuneComponentPtrVector();

}

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

void ImmuneSystem::draw_random_immune() {
  immune_component_->draw_random_immune();
}

double ImmuneSystem::get_lastest_immune_value() const {
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

  const auto last_immune_level = get_lastest_immune_value();
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
