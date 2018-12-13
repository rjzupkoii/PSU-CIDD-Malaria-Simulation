/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BurninMonthlyReporter.cpp
 * Author: Merlin
 * 
 * Created on April 11, 2017, 12:05 PM
 */

#include "BurninMonthlyReporter.h"
#include "Model.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"
#include "Strategies/MFTRebalancingStrategy.h"
#include "Strategies/NestedMFTStrategy.h"

BurninMonthlyReporter::BurninMonthlyReporter() {

}

BurninMonthlyReporter::~BurninMonthlyReporter() {

}

void BurninMonthlyReporter::initialize() {
  last_reported_NTF_ = 0;
  last_reported_mutants_ = 0;
  last_reported_clinical_episodes_ = 0;
}

void BurninMonthlyReporter::before_run() {

}

void BurninMonthlyReporter::begin_time_step() {

}

void BurninMonthlyReporter::monthly_report() {
  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    std::cout << Model::SCHEDULER->current_time() << "\t";

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

      std::cout << location_discounted_NTF << "\t";
      std::cout << NTF << "\t";
      std::cout << change_in_NTF << "\t";
      std::cout << change_in_clinical_episodes << "\t";
      std::cout << change_in_mutants << "\t";
      std::cout << "-1111" << "\t";

      if (Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()) {
        std::cout << 0 << "\t";
      } else {
        std::cout << Model::DATA_COLLECTOR->EIR_by_location_year()[location].back() << "\t";
      }
      std::cout << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->current_TF_by_location()[location] << "\t";
      std::cout << "-1111" << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10)*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 10, 15)*100 << "\t";
      std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 15, 45)*100 << "\t";
    }
    std::cout << "-1111" << "\t";
    for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
      std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[i] << "\t";
    }

    //output for MFT rebalencing
    if (Model::TREATMENT_STRATEGY->get_type()==IStrategy::NestedMFT) {
      // if (Model::CONFIG->strategy_db()[((NestedMFTStrategy *) Model::TREATMENT_STRATEGY)->switch_to_strategy_id]->get_type() ==
      //     IStrategy::MFTRebalancing) {
      //     std::cout << "-1111" << "\t";
      //     MFTRebalancingStrategy *s = (MFTRebalancingStrategy *) (Model::CONFIG->strategy_db()[((NestedMFTStrategy *) Model::TREATMENT_STRATEGY)->switch_to_strategy_id]);
      //     for (int i = 0; i < s->distribution.size(); i++) {
      //         std::cout << s->distribution[i] << "\t";
      //     }
      // }
    }

    std::cout << std::endl;
  }
}

void BurninMonthlyReporter::after_run() {

}
