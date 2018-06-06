/* 
 * File:   AdaptiveCyclingStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 4, 2013, 11:10 AM
 */

#include "AdaptiveCyclingStrategy.h"
#include "../Model.h"
#include "../MDC/ModelDataCollector.h"
#include "../Config.h"
#include <iostream>
#include <sstream>
#include "IStrategy.h"
#include "../Therapy.h"

AdaptiveCyclingStrategy::AdaptiveCyclingStrategy() : trigger_value_(0), delay_until_actual_trigger_(0), turn_off_days_(0), latest_switch_time_(0) {
    set_index(0);
}


AdaptiveCyclingStrategy::~AdaptiveCyclingStrategy() {
}

std::vector<Therapy*>& AdaptiveCyclingStrategy::get_therapy_list() {
    return therapy_list_;
}

void AdaptiveCyclingStrategy::add_therapy(Therapy* therapy) {
    therapy_list_.push_back(therapy);
}

void AdaptiveCyclingStrategy::switch_therapy() {
//    std::cout << "Switch from: " << index_ << "\t - to: " << index_ + 1;
    index_++;
    index_ %= therapy_list().size();

    Model::DATA_COLLECTOR->update_UTL_vector();
}

Therapy * AdaptiveCyclingStrategy::get_therapy(Person *person) {
    return therapy_list()[index_];
}

std::string AdaptiveCyclingStrategy::to_string() const {
    std::stringstream sstm;
    sstm << IStrategy::id << "-" << IStrategy::name << "-";
    for (int i = 0; i < therapy_list_.size() - 1; i++) {
        sstm << therapy_list_[i]->id() << ",";
    }
    sstm << therapy_list_[therapy_list_.size() - 1]->id();
    return sstm.str();
}

IStrategy::StrategyType AdaptiveCyclingStrategy::get_type() const {
    return IStrategy::AdaptiveCycling;
}

void AdaptiveCyclingStrategy::update_end_of_time_step() {

    if (Model::SCHEDULER->current_time() > Model::CONFIG->start_treatment_day() + Model::CONFIG->tf_window_size()) {
        if (Model::SCHEDULER->current_time() == latest_switch_time_) {
            switch_therapy();
            //            std::cout << to_string() << std::endl;
        } else {
            if (Model::DATA_COLLECTOR->current_TF_by_therapy()[get_therapy(nullptr)->id()] > trigger_value_) {
                if (Model::SCHEDULER->current_time() > (latest_switch_time_ + turn_off_days_)) {
                    latest_switch_time_ = Model::SCHEDULER->current_time() + delay_until_actual_trigger_;
                    //                    std::cout << "TF: " << Model::DATA_COLLECTOR->current_TF_by_therapy()[get_therapy()->id()] << std::endl;
                }
            }
        }
    }
}