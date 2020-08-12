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

void MMCReporter::initialize() { }

void MMCReporter::before_run() {
  // // std::cout << "MMC Reporter" << std::endl;
  // for (auto genotype : (*Model::CONFIG->genotype_db())){
  //   std::cout << *genotype.second << std::endl;
  // }

}

void MMCReporter::begin_time_step() { }


void MMCReporter::print_treatment_failure_rate_by_therapy() {
  for (auto tf_by_therapy : Model::DATA_COLLECTOR->current_tf_by_therapy()) {
    ss << tf_by_therapy << sep;
  }
}

void MMCReporter::print_ntf_by_location() {
  double sum_ntf = 0.0;
  ul pop_size = 0;
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];

  }

  ss << (sum_ntf * 100.0 / pop_size) << sep;
}

void MMCReporter::monthly_report() {
  ss << Model::SCHEDULER->current_time() << sep;
  ss << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << sep;
  ss << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << sep;
  ss << Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << sep;
  ss << Model::POPULATION->size() << sep;
  ss << group_sep;

  print_EIR_PfPR_by_location();
  ss << group_sep;
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << sep;
  }
  ss << group_sep;
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << sep;
  }
  ss << group_sep;

  output_genotype_frequency_3(
      Model::CONFIG->number_of_parasite_types(),
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>());

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
  ss << Model::RANDOM->seed() << sep << Model::CONFIG->number_of_locations() << sep;
  ss << Model::CONFIG->location_db()[0].beta << sep;
  ss << Model::CONFIG->location_db()[0].population_size << sep;
  print_EIR_PfPR_by_location();

  ss << group_sep;
  //output last strategy information
  ss << Model::TREATMENT_STRATEGY->id << sep;

  //output NTF
  const auto total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) /
                                   static_cast<double>(Constants::DAYS_IN_YEAR());

  auto sum_ntf = 0.0;
  ul pop_size = 0;
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];
  }

  ss << (sum_ntf * 100 / pop_size) / total_time_in_years << sep;
  ss << group_sep;

  ss << Model::DATA_COLLECTOR->cumulative_number_treatments_by_location()[0] << sep;
  ss << Model::DATA_COLLECTOR->cumulative_TF_by_location()[0] << sep;
  ss << Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[0] << sep;

  ss << group_sep;
  //print # mutation events of first 10 years
  int number_of_years = Model::DATA_COLLECTOR->number_of_mutation_events_by_year().size() >= 11 ? 11 :
                        Model::DATA_COLLECTOR->number_of_mutation_events_by_year().size();
  for (int i = 0; i < number_of_years; ++i) {
    ss << Model::DATA_COLLECTOR->number_of_mutation_events_by_year()[i] << sep;
  }
  CLOG(INFO, "summary_reporter") << ss.str();
  ss.str("");
}

void MMCReporter::print_EIR_PfPR_by_location() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    //
    // EIR
    if (Model::DATA_COLLECTOR->EIR_by_location_year()[loc].empty()) {
      ss << 0 << sep;
    } else {
      ss << Model::DATA_COLLECTOR->EIR_by_location_year()[loc].back() << sep;
    }

    //pfpr <5 and all
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 0, 5) * 100 << sep;
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 2, 10) * 100 << sep;
    ss << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << sep;
//    std::cout << Model::POPULATION->size() << "\t"
//              << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << std::endl;
  }
}


void MMCReporter::output_genotype_frequency_3(
    const int& number_of_genotypes,
    PersonIndexByLocationStateAgeClass* pi
) {
  auto sum1_all = 0.0;
  std::vector<double> result3_all(number_of_genotypes, 0.0);
  const auto number_of_locations = pi->vPerson().size();
  const auto number_of_age_classes = pi->vPerson()[0][0].size();

  for (auto loc = 0; loc < number_of_locations; loc++) {
    std::vector<double> result3(number_of_genotypes, 0.0);
    auto sum1 = 0.0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (auto ac = 0; ac < number_of_age_classes; ac++) {
        const auto size = pi->vPerson()[loc][hs][ac].size();
        for (auto i = 0ull; i < size; i++) {
          auto* person = pi->vPerson()[loc][hs][ac][i];

          if (!person->all_clonal_parasite_populations()->parasites()->empty()) {
            sum1 += 1;
            sum1_all += 1;
          }

          std::map<int, int> individual_genotype_map;

          for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
            const auto g_id = parasite_population->genotype()->genotype_id();
            if (individual_genotype_map.find(g_id) == individual_genotype_map.end()) {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] = 1;
            } else {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] += 1;
            }
          }

          for (const auto genotype : individual_genotype_map) {
            result3[genotype.first] += genotype.second /
                                       static_cast<double>(person->all_clonal_parasite_populations()
                                                                 ->parasites()
                                                                 ->size()
                                       );
            result3_all[genotype.first] += genotype.second / static_cast<double>(person
                ->all_clonal_parasite_populations()->parasites()->size());
          }
        }
      }
    }
    // output per location
    for (auto& i : result3) {
      i /= sum1;
      ss << i << sep;
    }
  }
}
