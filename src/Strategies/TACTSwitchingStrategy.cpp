/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TACTStrategy.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2017, 3:05 PM
 */

#include "TACTSwitchingStrategy.h"
#include "../Model.h"
#include "../Scheduler.h"
#include "../Config.h"

TACTSwitchingTStrategy::TACTSwitchingTStrategy() {
}

TACTSwitchingTStrategy::~TACTSwitchingTStrategy() {
}

std::string TACTSwitchingTStrategy::to_string() const {
    std::stringstream sstm;
    sstm << ACTIncreaseStrategy::to_string() << "-";
    sstm << TACT_switching_day_ << "-" << TACT_id_;

    return sstm.str();
}

void TACTSwitchingTStrategy::update_end_of_time_step() {
    if (Model::SCHEDULER->current_time() == TACT_switching_day_) {
        //        std::cout << to_string() << std::endl;
        //by defaults, tact will simply replace the first therapy in the MFT strategy        
        therapy_list()[0] = Model::CONFIG->therapy_db()[TACT_id_];
        //        std::cout << to_string() << std::endl;

    }
}

