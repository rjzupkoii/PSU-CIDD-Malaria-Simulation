/* 
 * File:   YearlyReporterV1.cpp
 * Author: Merlin
 * 
 * Created on January 13, 2015, 3:53 PM
 */

#include "YearlyReporterV1.h"
#include "../Model.h"
#include "../Scheduler.h"
#include "../Random.h"
#include "../Config.h"
#include "../MDC/ModelDataCollector.h"

YearlyReporterV1::YearlyReporterV1(const std::string &file_name) : file_name_(file_name) {

}


YearlyReporterV1::~YearlyReporterV1() {
}

void YearlyReporterV1::initialize() {
}

void YearlyReporterV1::before_run() {
    //    std::cout << Model::RANDOM->seed() << std::endl;
    fs_.open(file_name_.c_str(), std::fstream::out);
}

void YearlyReporterV1::begin_time_step() {
}

void YearlyReporterV1::after_time_step() {
    if ((Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) &&
        (((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) % 360) == 0)) {
        Model::DATA_COLLECTOR->perform_population_statistic();

        fs_ << ((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) / 360) << "\t";

        for (int age = 0; age < 80; age++) {
            fs_ << Model::DATA_COLLECTOR->popsize_by_location_age()[0][age] << "\t";
            fs_ << Model::DATA_COLLECTOR->number_of_deaths_by_location_age_year()[0][age] << "\t";
            fs_ << Model::DATA_COLLECTOR->number_of_malaria_deaths_by_location_age_year()[0][age] << "\t";
            fs_ << Model::DATA_COLLECTOR->number_of_untreated_cases_by_location_age_year()[0][age] << "\t";
            //            fs << Model::STATISTIC->number_of_treatments_by_location_age_year()[0][age] << "\t";
            for (int t_id = 0; t_id < 3; t_id++) {
                fs_ << Model::DATA_COLLECTOR->number_of_treatments_by_location_age_therapy_year()[0][age][t_id] << "\t";
                fs_ << Model::DATA_COLLECTOR->number_of_treatment_failures_by_location_age_therapy_year()[0][age][t_id]
                    << "\t";
            }
        }
        fs_ << std::endl;
    }
}

void YearlyReporterV1::after_run() {

    fs_.close();
}