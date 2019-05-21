/* 
 * File:   YearlyReporterV1.cpp
 * Author: Merlin
 * 
 * Created on January 13, 2015, 3:53 PM
 */

#include "YearlyReporterV1.h"
#include "Model.h"
#include "Scheduler.h"
#include "Core/Random.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Constants.h"

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

//TODO: consider add new method for yearly_reports
void YearlyReporterV1::monthly_report() {
  if ((Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) &&
      (((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day())%Constants::DAYS_IN_YEAR())==
          0)) {
    Model::DATA_COLLECTOR->perform_population_statistic();

    fs_ << ((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day())/
        Constants::DAYS_IN_YEAR())
        << "\t";

    for (int age = 0; age < 80; age++) {
      fs_ << Model::DATA_COLLECTOR->popsize_by_location_age()[0][age] << "\t";
      fs_ << Model::DATA_COLLECTOR->number_of_deaths_by_location_age_year()[0][age] << "\t";
      fs_ << Model::DATA_COLLECTOR->number_of_malaria_deaths_by_location_age_year()[0][age] << "\t";
      fs_ << Model::DATA_COLLECTOR->number_of_untreated_cases_by_location_age_year()[0][age] << "\t";
      //            fs << Model::STATISTIC->number_of_treatments_by_location_age_year()[0][age] << "\t";

    }
    fs_ << std::endl;
  }
}

void YearlyReporterV1::after_run() {

  fs_.close();
}