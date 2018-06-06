//
// Created by Nguyen Tran on 3/16/2018.
//

#include <sstream>
#include "MFTDifferentDistributionByLocationStrategy.h"
#include "../Therapy.h"
#include "../Model.h"
#include "../Random.h"
#include "../Person.h"
#include "../Config.h"

MFTDifferentDistributionByLocationStrategy::MFTDifferentDistributionByLocationStrategy() : distribution_by_location_() {

}

MFTDifferentDistributionByLocationStrategy::~MFTDifferentDistributionByLocationStrategy() {

}

void MFTDifferentDistributionByLocationStrategy::add_therapy(Therapy *therapy) {
    therapy_list_.push_back(therapy);
}

Therapy *MFTDifferentDistributionByLocationStrategy::get_therapy(Person *person) {

    double P = Model::RANDOM->random_flat(0.0, 1.0);
    int loc = person->location();

    double sum = 0;
    for (int i = 0; i < distribution_by_location_[loc].size(); i++) {
        sum += distribution_by_location_[loc][i];
        if (P <= sum) {
            return therapy_list()[i];
        }
    }

    return therapy_list()[therapy_list().size() - 1];
}

std::string MFTDifferentDistributionByLocationStrategy::to_string() const {
    std::stringstream sstm;
    sstm << IStrategy::id << "-" << IStrategy::name << "-";

    for (int i = 0; i < therapy_list_.size() - 1; i++) {
        std::cout << "hello" << therapy_list_[i]->id() << std::endl;
    }
    sstm << therapy_list_[therapy_list_.size() - 1]->id() << "-" << std::endl;;

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        sstm << "[";
        for (int i = 0; i < distribution_by_location_[loc].size() - 1; i++) {
            sstm << distribution_by_location_[loc][i] << ",";
        }
        sstm << distribution_by_location_[loc][therapy_list_.size() - 1] << "]" << std::endl;

    }

    return sstm.str();
}

IStrategy::StrategyType MFTDifferentDistributionByLocationStrategy::get_type() const {
    return MFTDifferentDistributionByLocation;
}

void MFTDifferentDistributionByLocationStrategy::update_end_of_time_step() {
    //do nothing here
}
