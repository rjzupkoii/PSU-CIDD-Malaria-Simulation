/* 
 * File:   NormalImmuneComponent.cpp
 * Author: nguyentran
 * 
 * Created on May 28, 2013, 11:06 AM
 */

#include "NonInfantImmuneComponent.h"
#include "Model.h"
#include "Core/Config/Config.h"


//OBJECTPOOL_IMPL(NonInfantImmuneComponent)

NonInfantImmuneComponent::NonInfantImmuneComponent(ImmuneSystem *immune_system) : ImmuneComponent(immune_system) {
}

NonInfantImmuneComponent::~NonInfantImmuneComponent() {
}

double NonInfantImmuneComponent::get_acquire_rate(const int &age) const {
  //    return FastImmuneComponent::acquireRate;

  return (age > 80) ? Model::CONFIG->immune_system_information().acquire_rate_by_age[80]
                    : Model::CONFIG->immune_system_information().acquire_rate_by_age[age];

}

double NonInfantImmuneComponent::get_decay_rate(const int &age) const {
  return Model::CONFIG->immune_system_information().decay_rate;
}