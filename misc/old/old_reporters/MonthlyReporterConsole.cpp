/* 
 * File:   MonthlyReporterConsole.cpp
 * Author: macpro
 * 
 * Created on July 11, 2016, 1:28 PM
 */

#include "MonthlyReporterConsole.h"
#include "Model.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"

MonthlyReporterConsole::MonthlyReporterConsole() {

}

MonthlyReporterConsole::~MonthlyReporterConsole() {
}

void MonthlyReporterConsole::initialize() {
  last_reported_NTF_ = 0;
  last_reported_mutants_ = 0;
  last_reported_clinical_episodes_ = 0;
}

void MonthlyReporterConsole::before_run() {

}

void MonthlyReporterConsole::begin_time_step() {

}

void MonthlyReporterConsole::monthly_report() {
  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    std::cout << Model::SCHEDULER->current_time() << "\t";
    std::cout << Model::DATA_COLLECTOR->AMU_per_parasite_pop() << "\t";
    //        std::cout << std::setw(COLUMN_WIDTH) << Model::STATISTIC->AFU() << "\t";

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

      std::cout << location_discounted_NTF << "\t";
      std::cout << NTF << "\t";
      std::cout << change_in_NTF << "\t";

      //            std::cout << Model::STATISTIC->cumulative_clinical_episodes_by_location()[location] << "\t";
      std::cout << change_in_clinical_episodes << "\t";

      //            std::cout << Model::STATISTIC->cumulative_mutants_by_location()[location] << "\t";
      std::cout << change_in_mutants << "\t";
      std::cout << Model::DATA_COLLECTOR->current_TF_by_location()[location] << "\t";
    }

    std::cout << "-1111" << "\t";
    //        std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[0]<< "\t";
    //        std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[16]<< "\t";
    //        std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[16]/(double)Model::DATA_COLLECTOR->resistance_tracker().total()<< "\t";
    //        std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[80] / (double) Model::DATA_COLLECTOR->resistance_tracker().total() << "\t";

    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().single_resistance_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().double_resistance_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().tripple_resistance_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().quadruple_resistance_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().quintuple_resistance_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().sum_fraction_resistance(
        Model::DATA_COLLECTOR->resistance_tracker().artemisinin_ids()) << "\t";
    std::cout << Model::DATA_COLLECTOR->resistance_tracker().total_resistance_frequency() << "\t";

    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      std::cout << "-1111\t";
      //            report_number_by_state(location, pi);
      std::cout << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10)*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 10, 15)*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 15, 45)*100 << "\t";
      if (Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()) {
        std::cout << 0 << "\t";
      } else {
        std::cout << Model::DATA_COLLECTOR->EIR_by_location_year()[location].back() << "\t";
      }

    }

    std::cout << Model::DATA_COLLECTOR->mean_moi() << "\t";
    std::cout << std::endl;
  }
}

void MonthlyReporterConsole::after_run() {

}