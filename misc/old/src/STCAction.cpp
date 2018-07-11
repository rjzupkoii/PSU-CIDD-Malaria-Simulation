/* 
 * File:   STCAction.cpp
 * Author: Merlin
 * 
 * Created on August 24, 2013, 3:40 PM
 */

#include "STCAction.h"
#include "Model.h"
#include "Scheduler.h"
#include <iostream>

STCAction::STCAction() {
}

STCAction::~STCAction() {
}

void STCAction::perform_weekly_action(const int& location) {
    std::cout << Model::SCHEDULER->current_time() << "\t STC Action on location " << location << std::endl;
}
