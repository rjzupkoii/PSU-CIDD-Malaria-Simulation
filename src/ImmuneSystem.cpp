/* 
 * File:   ImmuneSystem.cpp
 * Author: nguyentran
 * 
 * Created on May 27, 2013, 11:56 AM
 */

#include "ImmuneSystem.h"
#include <boost/foreach.hpp>
#include "ImmuneComponent.h"
#include "HelperFunction.h"
#include "Person.h"
#include "Model.h"
#include "Config.h"
#include "NonInfantImmuneComponent.h"
#include "InfantImmuneComponent.h"
#include <math.h>

OBJECTPOOL_IMPL(ImmuneSystem)

ImmuneSystem::ImmuneSystem(Person *p) : person_(p), increase_(false), immune_component_(nullptr) {
    //    immune_components_ = new ImmuneComponentPtrVector();

}

ImmuneSystem::~ImmuneSystem() {

    if (immune_component_ != nullptr) {
        DeletePointer<ImmuneComponent>(immune_component_);
    }
    assert(immune_component_ == nullptr);
    person_ = nullptr;
}

ImmuneComponent* ImmuneSystem::immune_component() const {
    return immune_component_;
}

void ImmuneSystem::set_immune_component(ImmuneComponent* value) {
    if (immune_component_ != value) {
        if (immune_component_ != nullptr) {
            DeletePointer<ImmuneComponent>(immune_component_);
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

double ImmuneSystem::get_parasite_size_after_t_days(const int& duration, const double &originalSize, const double &fitness) const {

    double lastImmuneLevel = get_lastest_immune_value();
    double temp = Model::CONFIG->immune_system_information().c_max * (1 - lastImmuneLevel) + Model::CONFIG->immune_system_information().c_min * lastImmuneLevel;


    double value = originalSize + duration * (log10(temp) + log10(fitness));
    return value;

}

const double mid_point = 0.4;

double ImmuneSystem::get_clinical_progression_probability() const {
    double immune = get_current_value();

    ImmuneSystemInformation isf = Model::CONFIG->immune_system_information();

    //    double PClinical = (isf.min_clinical_probability - isf.max_clinical_probability) * pow(immune, isf.immune_effect_on_progression_to_clinical) + isf.max_clinical_probability;

    //    const double p_m = 0.99;


    double PClinical = isf.max_clinical_probability / (1 + pow((immune / mid_point), isf.immune_effect_on_progression_to_clinical));

    //    std::cout << immune << "\t" << PClinical<< std::endl;
    return PClinical;
}

void ImmuneSystem::update() {
    immune_component_->update();
}