//
// Created by Nguyen Tran on 3/5/2018.
//

#include "BFMonthlyReporter.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Random.h"
#include "Strategies/IStrategy.h"
#include "Helpers/TimeHelpers.h"
#include "Constants.h"
#include "easylogging++.h"

BFMonthlyReporter::BFMonthlyReporter() = default;

BFMonthlyReporter::~BFMonthlyReporter() = default;

void BFMonthlyReporter::initialize() {}

void BFMonthlyReporter::before_run() {}


void BFMonthlyReporter::begin_time_step() {}

void BFMonthlyReporter::monthly_report() {
  //    ss << Model::SCHEDULER->calendar_date << std::endl;
  //    if (Model::SCHEDULER->current_time() % Model::CONFIG->report_frequency() == 0) {

  ss << Model::SCHEDULER->current_time() << "\t";
  ss << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << "\t";
  ss << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << "\t";
  ss << Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << "\t";
  print_PfPR_0_5_by_location();

  //        ss << "-1111\t";
  //
  //        print_monthly_incidence_by_location();
  //
  //        ss << "-1111" << "\t";
  //
  //        for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
  //            ss << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[i] << "\t";
  //        }
  //
  //        ss << "-1111" << "\t";
  //
  //        for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
  //            ss << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[i] << "\t";
  //        }
  //
  //        ss << "-1111" << "\t";
  //        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
  //            for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
  //                ss << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count_by_location()[loc][i]
  //                          << "\t";
  //            }
  //        }

  CLOG(INFO, "monthly_reporter") << ss.str();
  ss.str("");
}


void BFMonthlyReporter::after_run() {
  ss.str("");
  ss << Model::RANDOM->seed() << "\t" << Model::CONFIG->number_of_locations() << "\t";

  //output last strategy information
  ss << Model::TREATMENT_STRATEGY->id << "\t";

  //output NTF
  const auto total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) /
    static_cast<double>(Constants::DAYS_IN_YEAR());

  auto sum_ntf = 0.0;
  ul pop_size = 0;
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];
  }

  ss << (sum_ntf * 100 / pop_size) / total_time_in_years << "\t";

  //    ss << "-1111\t";
  //    for (int age = 0; age < 80; age++) {
  //        int sum = 0;
  //        for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
  //            sum += Model::DATA_COLLECTOR->number_of_treatments_by_location_age_year()[location][age];
  //
  //        }
  //        ss << sum << "\t";
  //    }


  CLOG(INFO, "summary_reporter") << ss.str();
  ss.str("");
}

void BFMonthlyReporter::print_PfPR_0_5_by_location() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 0, 5) * 100 << "\t";
  }
}

void BFMonthlyReporter::print_monthly_incidence_by_location() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << "\t";
  }

  ss << "-1111\t";

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << "\t";
  }

}
