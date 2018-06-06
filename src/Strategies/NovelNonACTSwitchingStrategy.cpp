/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NovelNonACTSwitchingStrategy.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2017, 3:00 PM
 */

#include "NovelNonACTSwitchingStrategy.h"
#include "MFTStrategy.h"
#include "../Model.h"
#include "../Config.h"
#include "../MDC/ModelDataCollector.h"
#include "../Scheduler.h"
#include <sstream>

NovelNonACTSwitchingStrategy::NovelNonACTSwitchingStrategy() {
}

NovelNonACTSwitchingStrategy::~NovelNonACTSwitchingStrategy() {
}

IStrategy::StrategyType NovelNonACTSwitchingStrategy::get_type() const {
    return IStrategy::NovelNonACTSwitching;
}


std::string NovelNonACTSwitchingStrategy::to_string() const {
    std::stringstream sstm;
    sstm << MFTStrategy::to_string() << "-";
    sstm << non_artemisinin_switching_day_ << "-" << non_art_therapy_id_ << "-" << fraction_non_art_replacement_;

    return sstm.str();
}

void NovelNonACTSwitchingStrategy::update_end_of_time_step() {
    if (Model::SCHEDULER->current_time() == non_artemisinin_switching_day_) {
//        std::cout << to_string() << std::endl;

        if (fraction_non_art_replacement_ > 0.0) {
            //collect the current TF
            //TODO: multiple location
            Model::DATA_COLLECTOR->TF_at_15() = Model::DATA_COLLECTOR->current_TF_by_location()[0];
            Model::DATA_COLLECTOR->single_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().single_resistance_ids());
            Model::DATA_COLLECTOR->double_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().double_resistance_ids());
            Model::DATA_COLLECTOR->triple_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().tripple_resistance_ids());
            Model::DATA_COLLECTOR->quadruple_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().quadruple_resistance_ids());
            Model::DATA_COLLECTOR->quintuple_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().quintuple_resistance_ids());
            Model::DATA_COLLECTOR->art_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(Model::DATA_COLLECTOR->resistance_tracker().artemisinin_ids());
            Model::DATA_COLLECTOR->total_resistance_frequency_at_15() = Model::DATA_COLLECTOR->resistance_tracker().calculate_total_resistance_frequency();


            //switch therapy 2 to therapy 3
            int number_of_therapies = therapy_list().size();

            //switch the first therapy in the list
            therapy_list()[0] = Model::CONFIG->therapy_db()[non_art_therapy_id_];
            //change the distribution         
            distribution()[0] = fraction_non_art_replacement_;

            for (int i = 1; i < number_of_therapies; i++) {
                distribution()[i] = (1 - fraction_non_art_replacement_) / (double) (number_of_therapies - 1);
            }

//            std::cout << to_string() << std::endl;
        }
    }
}
