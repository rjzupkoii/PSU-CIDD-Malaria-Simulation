/* 
 * File:   MFTStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 4, 2013, 11:09 AM
 */

#include "MFTStrategy.h"
#include "../Random.h"
#include "../Model.h"
#include <assert.h>
#include <sstream>
#include "IStrategy.h"
#include "../Therapy.h"

MFTStrategy::MFTStrategy() : distribution_() {
    //    if (config != NULL) {
    //        if (config->model() != NULL) {
    //            random_ = config->model()->random();
    //        }
    //    }

}

MFTStrategy::~MFTStrategy() {
}

void MFTStrategy::add_therapy(Therapy* therapy) {
    therapy_list_.push_back(therapy);
}

Therapy * MFTStrategy::get_therapy(Person *person) {

    double P = Model::RANDOM->random_flat(0.0, 1.0);

    double sum = 0;
    for (int i = 0; i < distribution_.size(); i++) {
        sum += distribution_[i];
        if (P <= sum) {       
            return therapy_list()[i];
        }
    }

    return therapy_list()[therapy_list().size() - 1];
}

std::string MFTStrategy::to_string() const {
    std::stringstream sstm;
    sstm << IStrategy::id << "-" << IStrategy::name << "-";

    for (int i = 0; i < therapy_list_.size() - 1; i++) {
        sstm << therapy_list_[i]->id() << ",";
    }
    sstm << therapy_list_[therapy_list_.size() - 1]->id() << "-";

    for (int i = 0; i < distribution_.size() - 1; i++) {
        sstm << distribution_[i] << ",";
    }
    sstm << distribution_[therapy_list_.size() - 1];

    return sstm.str();
}

IStrategy::StrategyType MFTStrategy::get_type() const {
    return IStrategy::MFT;
}

void MFTStrategy::update_end_of_time_step() {
    //do nothing here
}