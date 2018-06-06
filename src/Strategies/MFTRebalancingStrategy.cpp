/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SmartMFTStrategy.cpp
 * Author: Merlin
 * 
 * Created on August 25, 2017, 11:57 AM
 */

#include "MFTRebalancingStrategy.h"
#include "../Model.h"
#include "../Config.h"
#include "../MDC/ModelDataCollector.h"
#include "../Therapy.h"

MFTRebalancingStrategy::MFTRebalancingStrategy() : next_update_time_(-1), latest_adjust_distribution_time_(-1) {
}

MFTRebalancingStrategy::~MFTRebalancingStrategy() {

}

IStrategy::StrategyType MFTRebalancingStrategy::get_type() const {
    return IStrategy::MFTRebalancing;
}

std::string MFTRebalancingStrategy::to_string() const {
    std::stringstream sstm;
    //    sstm << IStrategy::id << "-" << IStrategy::name << "-";
    //
    //    for (int i = 0; i < therapy_list().size() - 1; i++) {
    //        sstm << therapy_list()[i]->id() << ",";
    //    }
    //    sstm << therapy_list()[therapy_list().size() - 1]->id() << "-";
    //
    //    for (int i = 0; i < distribution().size() - 1; i++) {
    //        sstm << distribution()[i] << ",";
    //    }
    //    sstm << distribution()[therapy_list().size() - 1] << "-" << update_distribution_duration_;
    sstm << MFTStrategy::to_string() << "-" << update_duration_after_rebalancing_;
    return sstm.str();
}

void MFTRebalancingStrategy::update_end_of_time_step() {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_treatment_day()) {
        if (Model::SCHEDULER->current_time() == latest_adjust_distribution_time_) {
            //adjust distribution
            for (int i = 0; i < distribution().size(); i++) {
                distribution()[i] = next_distribution_[i];
            }

            next_update_time_ = Model::SCHEDULER->current_time() + update_duration_after_rebalancing_;
//            std::cout << to_string() << std::endl;
        } else {
            if (Model::SCHEDULER->current_time() == next_update_time_) {
                double sum = 0;
                for (int i = 0; i < distribution().size(); i++) {
                    //            std::cout << Model::DATA_COLLECTOR->current_TF_by_therapy()[therapy_list()[i]->id()] << "-";
                    if (Model::DATA_COLLECTOR->current_TF_by_therapy()[therapy_list()[i]->id()] < 0.05) {
                        next_distribution_[i] = 1.0 / 0.05;
                    } else {
                        next_distribution_[i] = 1.0 / Model::DATA_COLLECTOR->current_TF_by_therapy()[therapy_list()[i]->id()];
                    }
                    sum += next_distribution_[i];
                }

                for (int i = 0; i < distribution().size(); i++) {
                    next_distribution_[i] = next_distribution_[i] / sum;
                }

                latest_adjust_distribution_time_ = Model::SCHEDULER->current_time() + delay_until_actual_trigger_;
            }
        }
    }
}

