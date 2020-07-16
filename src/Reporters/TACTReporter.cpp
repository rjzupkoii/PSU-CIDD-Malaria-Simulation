//
// Created by nguyentd on 7/8/2020.
//

#include <Model.h>
#include <MDC/ModelDataCollector.h>
#include <Core/Config/Config.h>
#include <Population/Population.h>
#include <Population/Properties/PersonIndexByLocationStateAgeClass.h>
#include <Population/SingleHostClonalParasitePopulations.h>
#include "TACTReporter.h"
#include "easylogging++.h"
#include "ReporterUtils.h"
#include <Strategies/IStrategy.h>
#include <Strategies/NestedMFTStrategy.h>

void TACTReporter::initialize() {

}

void TACTReporter::before_run() {
  // output header for csv file
  ss << "TIME" << sep
     << "PFPR" << sep
     << "MUTATIONS" << sep
     << "NUMBER_OF_TREATMENTS" << sep
     << "NUMBER_OF_TREATMENT_FAILURES" << sep
     << "NUMBER_OF_SYMPTOMATIC_CASES" << sep;
  for (auto i = 0; i < Model::CONFIG->genotype_db()->size(); i++) {
    ss << "GENOTYPE_ID_" << i << sep;
  }
  ss << group_sep;
  for (auto i = 0; i < Model::DATA_COLLECTOR->current_tf_by_therapy().size(); i++) {
    ss << "TF_THERAPY_" << i << sep;
  }
  ss << "AVERAGE_TF_60" << sep;
  ss << "PUBLIC_FRACTION" << sep;
  ss << "PRIVATE_FRACTION";
  CLOG(INFO, "monthly_reporter") << ss.str();
  ss.str("");

}

void TACTReporter::monthly_report() {
  ss << Model::SCHEDULER->current_time() << sep;

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << sep;
  }

  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::DATA_COLLECTOR->monthly_number_of_mutation_events_by_location()[loc] << sep;
    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << sep;
    ss << Model::DATA_COLLECTOR->monthly_number_of_TF_by_location()[loc] << sep;
    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << sep;
  }

  output_genotype_frequency3(
      ss, Model::CONFIG->number_of_parasite_types(),
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>());

  ss << group_sep;

  for (auto tf_by_therapy : Model::DATA_COLLECTOR->current_tf_by_therapy()) {
    ss << tf_by_therapy << sep;
  }

  ss << Model::DATA_COLLECTOR->current_TF_by_location()[0] << sep;

  if (Model::TREATMENT_STRATEGY->get_type() == IStrategy::NestedMFT) {
    ss << dynamic_cast<NestedMFTStrategy*>(Model::TREATMENT_STRATEGY)->distribution[0] << sep;
    ss << dynamic_cast<NestedMFTStrategy*>(Model::TREATMENT_STRATEGY)->distribution[1];
  }


  CLOG(INFO, "monthly_reporter") << ss.str();
  ss.str("");
}

void TACTReporter::after_run() {
  ss.str("");
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
    ss << Model::CONFIG->location_db()[loc].beta << sep;
    if (Model::DATA_COLLECTOR->EIR_by_location_year()[loc].empty()) {
      ss << 0 << sep;
    } else {
      ss << Model::DATA_COLLECTOR->EIR_by_location_year()[loc].back() << sep;
    }
    ss << Model::TREATMENT_COVERAGE->p_treatment_less_than_5[0] << sep;
    ss << Model::DATA_COLLECTOR->cumulative_number_treatments_by_location()[loc] << sep;
    ss << Model::DATA_COLLECTOR->cumulative_TF_by_location()[loc] << sep;
    ss << Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[loc] << sep;
    ss << "FLT" << sep;
    ss << "TACT" << sep;
    ss << "importation" << sep;
  }

  CLOG(INFO, "summary_reporter") << ss.str();
  ss.str("");
}

void TACTReporter::begin_time_step() {

}

void TACTReporter::output_genotype_frequency3(
    std::stringstream& ss,
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

