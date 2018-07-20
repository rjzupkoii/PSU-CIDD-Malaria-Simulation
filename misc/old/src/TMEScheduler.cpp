/* 
 * File:   TMEScheduler.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2013, 1:38 PM
 */

#include "TMEScheduler.h"
#include "CSVRow.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "TMEActionFactory.h"
#include "TMEAction.h"
#include <iostream>     // std::cout
#include <fstream>   

TMEScheduler::TMEScheduler(Model* model) : model_(model), tme_file_name_("tme.txt") {
}

TMEScheduler::~TMEScheduler() {
}

void TMEScheduler::initialize() {
    //    read_csv(tme_file_name_);
}

std::istream& operator>>(std::istream& str, CSVRow& data) {
    data.read_next_row(str);
    return str;
}

void TMEScheduler::read_csv(const std::string& file_name) {
    schedule_.clear();
    std::ifstream file(file_name.c_str());

    int week = 0;
    CSVRow row;


    while (file >> row) {
        StringVector weekly_schedule;
        //1 line is a week
        //1 cell is a village/location
        //        std::cout << row.size() << std::endl;
        if (row.size() < Model::CONFIG->number_of_locations()) {
            //empty in put or do nothing at that week for other villages

        }
        for (int i = 0; i < Model::CONFIG->number_of_locations(); i++) {
            weekly_schedule.push_back(row[i]);

        }
        schedule_.push_back(weekly_schedule);
        week++;
    }
}

void TMEScheduler::print() {
    for (int i = 0; i < schedule_.size(); i++) {
        for (int j = 0; j < Model::CONFIG->number_of_locations(); j++) {
            std::cout << schedule_[i][j] << "\t";
        }
        std::cout << std::endl;

    }
}

void TMEScheduler::check_and_perform_TME_Actions() {
    // to do that we need to define the time to start the TME is at day 6000 (from the input file and this day is Monday??, the begining of the year???) in the simulation
    // 
    //if current day is sunday && next week have action for village i
    // perform that actions
    if (Model::CONFIG->tme_info().tme_starting_day == 0) return;
    
    int starting_day_in_week = (Model::CONFIG->tme_info().tme_starting_day - 1) % 7; //sunday

    int current_week = (Model::SCHEDULER->current_time() + 1 - Model::CONFIG->tme_info().tme_starting_day) / 7;

    if ((current_week >= 0) && ((Model::SCHEDULER->current_time() % 7) == starting_day_in_week) && (current_week < schedule_.size())) {
        for (int location = 0; location < schedule_[current_week].size(); location++) {
            if (schedule_[current_week][location] != ".") {
                std::string action_name = schedule_[current_week][location];
                TMEAction* action = TMEActionFactory::Get()->create_action(action_name);
                action->perform_weekly_action(location);
            }

        }
    }




}