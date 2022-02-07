//
// Created by Nguyen Tran on 3/5/2018.
//

#include "MMCReporter.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Random.h"
#include "Strategies/IStrategy.h"
#include "Helpers/TimeHelpers.h"
#include "Constants.h"
#include "easylogging++.h"
#include "date/date.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "ReporterUtils.h"
// #include "Parasites/GenotypeDatabase.h"

MMCReporter::MMCReporter() = default;

void MMCReporter::before_run() {
  // // std::cout << "MMC Reporter" << std::endl;
  // for (auto genotype : (*Model::CONFIG->genotype_db())){
  //   std::cout << *genotype.second << std::endl;
  // }

}

void MMCReporter::begin_time_step() {}

void MMCReporter::print_genotype_frequency() {
  // (1) number of parasite-positive individuals carrying genotype X / total number of parasite-positive
  // individuals
  // (2) number of clonal parasite populations carrying genotype X / total number of clonal parasite
  // populations
  // (3) weighted number of parasite-positive individuals carrying genotype X / total number of
  // parasite-positive individuals (the weights for each person describe the fraction of their clonal
  // populations carrying genotype X; e.g. an individual host with five clonal infections two of which
  // carry genotype X would be given a weight of 2/5).

  ReporterUtils::output_genotype_frequency3(ss, Model::CONFIG->number_of_parasite_types(),
                                            Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>());

}

void MMCReporter::print_treatment_failure_rate_by_therapy() {
  for (auto tf_by_therapy : Model::DATA_COLLECTOR->current_tf_by_therapy()) {
    ss << tf_by_therapy << Tsv::sep;
  }
}

void MMCReporter::print_ntf_by_location() {
  double sum_ntf = 0.0;
  ul pop_size = 0;
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];
    
  }

  ss << (sum_ntf * 100.0 / pop_size) << Tsv::sep;
}

void MMCReporter::monthly_report() {
  ss << Model::SCHEDULER->current_time() << Tsv::sep;
  ss << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << Tsv::sep;
  ss << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << Tsv::sep;
  ss << Model::CONFIG->seasonal_info()->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << Tsv::sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << Tsv::sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << Tsv::sep;
  ss << Model::POPULATION->size() << Tsv::sep;
  ss << group_sep;

  print_EIR_PfPR_by_location();
  ss << group_sep;
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << Tsv::sep;
  }
  ss << group_sep;
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << Tsv::sep;
  }
  ss << group_sep;
  print_genotype_frequency();
  ss << group_sep;
  print_ntf_by_location();
  ss << group_sep;
  print_treatment_failure_rate_by_therapy();
  ss << Model::DATA_COLLECTOR->current_TF_by_location()[0];
  CLOG(INFO, "monthly_reporter") << ss.str();
  ss.str("");
}

void MMCReporter::after_run() {
  ss.str("");
  ss << Model::RANDOM->seed() << Tsv::sep << Model::CONFIG->number_of_locations() << Tsv::sep;
  ss << Model::CONFIG->location_db()[0].beta << Tsv::sep;
  ss << Model::CONFIG->location_db()[0].population_size << Tsv::sep;
  print_EIR_PfPR_by_location();

  ss << group_sep;
  //output last strategy information
  ss << Model::TREATMENT_STRATEGY->id << Tsv::sep;

  ss << calculate_treatment_failures() << Tsv::sep;

  ss << group_sep;
  //print # mutation events of first 10 years
  int number_of_years = Model::DATA_COLLECTOR->number_of_mutation_events_by_year().size() >= 11 ? 11 :
                        Model::DATA_COLLECTOR->number_of_mutation_events_by_year().size();
  for (int i = 0; i < number_of_years; ++i) {
    ss << Model::DATA_COLLECTOR->number_of_mutation_events_by_year()[i] << Tsv::sep;
  }
  CLOG(INFO, "summary_reporter") << ss.str();
  ss.str("");
}

void MMCReporter::print_EIR_PfPR_by_location() {
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    //
    // EIR
    if (Model::DATA_COLLECTOR->EIR_by_location_year()[loc].empty()) {
      ss << 0 << Tsv::sep;
    } else {
      ss << Model::DATA_COLLECTOR->EIR_by_location_year()[loc].back() << Tsv::sep;
    }

    //pfpr <5 and all
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 0, 5) * 100 << Tsv::sep;
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 2, 10) * 100 << Tsv::sep;
    ss << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << Tsv::sep;
//    std::cout << Model::POPULATION->size() << "\t"
//              << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << std::endl;
  }
}
//
//void MMCReporter::print_monthly_incidence_by_location() {
//  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
//    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << sep;
//  }
//
//  ss << group_sep;
//
//  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
//    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << sep;
//  }
//}
