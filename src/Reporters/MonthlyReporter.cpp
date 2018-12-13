/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MonthlyReporter.cpp
 * Author: Merlin
 * 
 * Created on May 10, 2016, 2:16 PM
 */

#include "MonthlyReporter.h"
#include "Model.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"

MonthlyReporter::MonthlyReporter(const std::string &file_name) : file_name_(file_name) {

}

MonthlyReporter::~MonthlyReporter() {
}

void MonthlyReporter::initialize() {
  last_reported_NTF_ = 0;
  last_reported_mutants_ = 0;
  last_reported_clinical_episodes_ = 0;
}

void MonthlyReporter::before_run() {
  fs_.open(file_name_.c_str(), std::fstream::out);
}

void MonthlyReporter::begin_time_step() {

}

void MonthlyReporter::monthly_report() {
  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    fs_ << Model::SCHEDULER->current_time() << "\t";
    fs_ << Model::DATA_COLLECTOR->AMU_per_parasite_pop() << "\t";
    //        fs_ << std::setw(COLUMN_WIDTH) << Model::STATISTIC->AFU() << "\t";

    //        double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) / (double) Constants::DAYS_IN_YEAR();
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      double location_discounted_NTF = Model::DATA_COLLECTOR->cumulative_discounted_NTF_by_location()[location];
      double NTF = Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
      double change_in_NTF =
          Model::DATA_COLLECTOR->cumulative_discounted_NTF_by_location()[location] - last_reported_NTF_;
      int change_in_clinical_episodes =
          Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[location] -
              last_reported_clinical_episodes_;
      int change_in_mutants =
          Model::DATA_COLLECTOR->cumulative_mutants_by_location()[location] - last_reported_mutants_;

      last_reported_mutants_ = Model::DATA_COLLECTOR->cumulative_mutants_by_location()[location];
      last_reported_clinical_episodes_ = Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[location];
      last_reported_NTF_ = Model::DATA_COLLECTOR->cumulative_discounted_NTF_by_location()[location];

      //            location_discounted_NTF /= total_time_in_years;

      fs_ << location_discounted_NTF << "\t";
      fs_ << NTF << "\t";
      fs_ << change_in_NTF << "\t";

      //            fs_ << Model::STATISTIC->cumulative_clinical_episodes_by_location()[location] << "\t";
      fs_ << change_in_clinical_episodes << "\t";

      //            fs_ << Model::STATISTIC->cumulative_mutants_by_location()[location] << "\t";
      fs_ << change_in_mutants << "\t";
      fs_ << Model::DATA_COLLECTOR->current_TF_by_location()[location] << "\t";
    }

    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().single_resistance_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().double_resistance_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().tripple_resistance_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().quadruple_resistance_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().quintuple_resistance_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().artemisinin_ids()) << "\t";
    fs_ << Model::DATA_COLLECTOR->resistance_tracker().total_resistance_frequency() << "\t";

    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      fs_ << "-1111\t";
      //            report_number_by_state(location, pi);
      fs_ << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]*100 << "\t";
      fs_ << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10)*100 << "\t";
      fs_ << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 10, 15)*100 << "\t";
      fs_ << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 15, 45)*100 << "\t";
      if (Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()) {
        fs_ << 0 << "\t";
      } else {
        fs_ << Model::DATA_COLLECTOR->EIR_by_location_year()[location].back() << "\t";
      }

    }

    fs_ << Model::DATA_COLLECTOR->mean_moi() << "\t";

    fs_ << std::endl;
  }
}

void MonthlyReporter::after_run() {

  fs_.close();
}

