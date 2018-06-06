/* 
 * File:   InfantImmuneComponent.cpp
 * Author: nguyentran
 * 
 * Created on May 28, 2013, 11:06 AM
 */

#include "InfantImmuneComponent.h"
#include "Model.h"
#include "Person.h"
#include "ImmuneSystem.h"
#include <math.h>

//OBJECTPOOL_IMPL(InfantImmuneComponent)

InfantImmuneComponent::InfantImmuneComponent(ImmuneSystem* immune_system) : ImmuneComponent(immune_system) {
}

InfantImmuneComponent::~InfantImmuneComponent() {
}

double InfantImmuneComponent::get_acquire_rate(const int &age) const {
    return 0;
}

double InfantImmuneComponent::get_decay_rate(const int &age) const {
    return 0.0315;
}

double InfantImmuneComponent::get_current_value() {
    int currentTime = Model::SCHEDULER->current_time();
    double temp = 0.0;
    if (immune_system() != nullptr) {
        if (immune_system()->person() != nullptr) {
            int duration = currentTime - immune_system()->person()->latest_update_time();
            //decrease I(t) = I0 * e ^ (-b2*t);
            temp = latest_value() * exp(-get_decay_rate(0) * duration);
        }
    }
    return temp;
}