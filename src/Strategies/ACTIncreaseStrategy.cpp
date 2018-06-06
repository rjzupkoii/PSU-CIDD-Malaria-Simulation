/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ACTIncreaseStrategy.cpp
 * Author: Merlin
 * 
 * Created on April 26, 2017, 11:20 AM
 */

#include "ACTIncreaseStrategy.h"
#include "../Random.h"
#include "../Model.h"
#include <assert.h>
#include "../Therapy.h"
#include "../Config.h"
#include <sstream>

ACTIncreaseStrategy::ACTIncreaseStrategy() {
}

ACTIncreaseStrategy::~ACTIncreaseStrategy() {
}

void ACTIncreaseStrategy::add_therapy(Therapy* therapy) {
    therapy_list_.push_back(therapy);
}

Therapy * ACTIncreaseStrategy::get_therapy(Person *person) {
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

std::string ACTIncreaseStrategy::to_string() const {
    std::stringstream sstm;
    sstm << IStrategy::id << "-" << IStrategy::name << "-";
    for (int i = 0; i < therapy_list_.size() - 1; i++) {
        sstm << therapy_list_[i]->id() << ",";
    }
    sstm << therapy_list_[therapy_list_.size() - 1]->id() << "-";

    for (int i = 0; i < start_distribution_.size() - 1; i++) {
        sstm << start_distribution_[i] << ",";
    }
    sstm << start_distribution_[therapy_list_.size() - 1] << "-";


    for (int i = 0; i < end_distribution_.size() - 1; i++) {
        sstm << end_distribution_[i] << ",";
    }
    sstm << end_distribution_[therapy_list_.size() - 1] << "-";

    for (int i = 0; i < distribution_.size() - 1; i++) {
        sstm << distribution_[i] << ",";
    }
    sstm << distribution_[therapy_list_.size() - 1];

    return sstm.str();
}

IStrategy::StrategyType ACTIncreaseStrategy::get_type() const {
    return IStrategy::ACTIncreaseOvertime;
}

void ACTIncreaseStrategy::update_end_of_time_step() {
    if (Model::SCHEDULER->current_time() % 30 == 0) {
        adjustDisttribution(Model::SCHEDULER->current_time(), Model::CONFIG->total_time());
//        std::cout << to_string() << std::endl;
    }
}

void ACTIncreaseStrategy::adjustDisttribution(int time, int totaltime) {

    double dACT = ((end_distribution_[0] - start_distribution_[0]) * time) / totaltime + start_distribution_[0];

    distribution_[0] = dACT;
    double otherD = (1 - dACT) / (distribution_.size() - 1);
    for (int i = 1; i < distribution_.size(); i++) {
        distribution_[i] = otherD;
    }
}