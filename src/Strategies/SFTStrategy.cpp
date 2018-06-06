/* 
 * File:   SFTStrategy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 8:00 PM
 */

#include <assert.h>
#include "SFTStrategy.h"
#include "../Therapy.h"
#include "IStrategy.h"
#include <sstream>

SFTStrategy::SFTStrategy() {
}

SFTStrategy::~SFTStrategy() {
}

std::vector<Therapy*>& SFTStrategy::get_therapy_list() {
    return therapy_list_;
}

void SFTStrategy::add_therapy(Therapy* therapy) {
    therapy_list_.push_back(therapy);
}

bool SFTStrategy::is_strategy(const std::string& sName) {
    return (IStrategy::name == sName);
}

Therapy * SFTStrategy::get_therapy(Person *person) {
    return therapy_list_[0];
}

std::string SFTStrategy::to_string() const {
    std::stringstream sstm;
    sstm << IStrategy::id << "-" << IStrategy::name << "-" << therapy_list_[0]->id();
    return sstm.str();
}

IStrategy::StrategyType SFTStrategy::get_type() const {
    return IStrategy::SFT;
}

void SFTStrategy::update_end_of_time_step() {
    //do nothing here
}