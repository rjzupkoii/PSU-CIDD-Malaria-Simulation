/*
 * File:   Statistic.cpp
 * Author: Merlin
 * 
 * Created on July 9, 2013, 2:28 PM
 */

#include <numeric>
#include <cmath>
#include <algorithm>
#include "ModelDataCollector.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Population/Person.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Population/Population.h"
#include "Population/ImmuneSystem.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Therapies/SCTherapy.h"
#include "Population/ClonalParasitePopulation.h"
#include "Constants.h"

ModelDataCollector::ModelDataCollector(Model* model) : model_(model), current_utl_duration_(0),
                                                       AMU_per_parasite_pop_(0),
                                                       AMU_per_person_(0), AMU_for_clinical_caused_parasite_(0),
                                                       AFU_(0), discounted_AMU_per_parasite_pop_(0),
                                                       discounted_AMU_per_person_(0),
                                                       discounted_AMU_for_clinical_caused_parasite_(0),
                                                       discounted_AFU_(0), tf_at_15_(0),
                                                       single_resistance_frequency_at_15_(0),
                                                       double_resistance_frequency_at_15_(0),
                                                       triple_resistance_frequency_at_15_(0),
                                                       quadruple_resistance_frequency_at_15_(0),
                                                       quintuple_resistance_frequency_at_15_(0),
                                                       art_resistance_frequency_at_15_(0),
                                                       total_resistance_frequency_at_15_(0), mean_moi_(0) { }

ModelDataCollector::~ModelDataCollector()
= default;

void ModelDataCollector::initialize() {
  if (model_ != nullptr) {
    popsize_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    popsize_residence_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    blood_slide_prevalence_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    blood_slide_prevalence_by_location_age_group_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(
            Model::CONFIG->number_of_age_classes(),
            0.0
        ));
    blood_slide_number_by_location_age_group_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(
            Model::CONFIG->number_of_age_classes(),
            0.0
        ));
    blood_slide_prevalence_by_location_age_group_by_5_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(
            Model::CONFIG->number_of_age_classes(),
            0.0
        ));
    blood_slide_number_by_location_age_group_by_5_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(
            Model::CONFIG->number_of_age_classes(),
            0.0
        ));
    fraction_of_positive_that_are_clinical_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    popsize_by_location_hoststate_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Person::NUMBER_OF_STATE, 0));
    popsize_by_location_age_class_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->number_of_age_classes(), 0));
    popsize_by_location_age_class_by_5_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->number_of_age_classes(), 0));

    total_immune_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    total_immune_by_location_age_class_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(Model::CONFIG->number_of_age_classes(), 0.0));

    total_number_of_bites_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0);
    total_number_of_bites_by_location_year_ = LongVector(Model::CONFIG->number_of_locations(), 0);
    person_days_by_location_year_ = LongVector(Model::CONFIG->number_of_locations(), 0);

    EIR_by_location_year_ = DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
    EIR_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

    cumulative_clinical_episodes_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0);
    cumulative_clinical_episodes_by_location_age_ = LongVector2(
        Model::CONFIG->number_of_locations(),
        LongVector(100, 0));
    cumulative_clinical_episodes_by_location_age_group_ = LongVector2(
        Model::CONFIG->number_of_locations(),
        LongVector(
            Model::CONFIG->number_of_age_classes(),
            0
        ));

    average_number_biten_by_location_person_ = DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
    percentage_bites_on_top_20_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

    cumulative_discounted_NTF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    cumulative_NTF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    cumulative_TF_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0.0);
    cumulative_number_treatments_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0.0);

    today_TF_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    today_number_of_treatments_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    today_RITF_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    total_number_of_treatments_60_by_location_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->tf_window_size(), 0));
    total_RITF_60_by_location_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->tf_window_size(), 0));
    total_TF_60_by_location_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->tf_window_size(), 0));

    current_RITF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    current_TF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

    cumulative_mutants_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    current_utl_duration_ = 0;
    UTL_duration_ = IntVector();

    number_of_treatments_with_therapy_ID_ = IntVector(Model::CONFIG->therapy_db().size(), 0);
    number_of_treatments_success_with_therapy_ID_ = IntVector(Model::CONFIG->therapy_db().size(), 0);
    number_of_treatments_fail_with_therapy_ID_ = IntVector(Model::CONFIG->therapy_db().size(), 0);

    AMU_per_parasite_pop_ = 0;
    AMU_per_person_ = 0;
    AMU_for_clinical_caused_parasite_ = 0;

    AFU_ = 0;

    discounted_AMU_per_parasite_pop_ = 0;
    discounted_AMU_per_person_ = 0;
    discounted_AMU_for_clinical_caused_parasite_ = 0;

    discounted_AFU_ = 0;

    multiple_of_infection_by_location_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(number_of_reported_MOI, 0));

    current_EIR_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
    last_update_total_number_of_bites_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0);

    last_10_blood_slide_prevalence_by_location_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(10, 0.0));
    last_10_fraction_positive_that_are_clinical_by_location_ = DoubleVector2(
        Model::CONFIG->number_of_locations(),
        DoubleVector(10, 0.0));
    last_10_fraction_positive_that_are_clinical_by_location_age_class_ = DoubleVector3(
        Model::CONFIG->number_of_locations(),
        DoubleVector2(Model::CONFIG->number_of_age_classes(), DoubleVector(10, 0.0)));
    last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_ = DoubleVector3(
        Model::CONFIG->number_of_locations(),
        DoubleVector2(Model::CONFIG->number_of_age_classes(), DoubleVector(10, 0.0)));
    total_parasite_population_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    number_of_positive_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    total_parasite_population_by_location_age_group_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(
            Model::CONFIG->number_of_age_classes(),
            0
        ));
    number_of_positive_by_location_age_group_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->number_of_age_classes(), 0));
    number_of_clinical_by_location_age_group_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->number_of_age_classes(), 0));
    number_of_clinical_by_location_age_group_by_5_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(
            Model::CONFIG->number_of_age_classes(),
            0
        ));
    number_of_death_by_location_age_group_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(Model::CONFIG->number_of_age_classes(), 0));

    number_of_untreated_cases_by_location_age_year_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(80, 0));
    number_of_treatments_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));
    number_of_deaths_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));
    number_of_malaria_deaths_by_location_age_year_ = IntVector2(
        Model::CONFIG->number_of_locations(),
        IntVector(80, 0));

    popsize_by_location_age_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));

    tf_at_15_ = 0;
    single_resistance_frequency_at_15_ = 0;
    double_resistance_frequency_at_15_ = 0;
    triple_resistance_frequency_at_15_ = 0;
    quadruple_resistance_frequency_at_15_ = 0;
    quintuple_resistance_frequency_at_15_ = 0;
    art_resistance_frequency_at_15_ = 0;
    total_resistance_frequency_at_15_ = 0;

    total_number_of_treatments_60_by_therapy_ = IntVector2(
        Model::CONFIG->therapy_db().size(),
        IntVector(Model::CONFIG->tf_window_size(), 0));
    total_tf_60_by_therapy_ = IntVector2(
        Model::CONFIG->therapy_db().size(),
        IntVector(Model::CONFIG->tf_window_size(), 0));
    current_tf_by_therapy_ = DoubleVector(Model::CONFIG->therapy_db().size(), 0.0);
    today_tf_by_therapy_ = IntVector(Model::CONFIG->therapy_db().size(), 0);
    today_number_of_treatments_by_therapy_ = IntVector(Model::CONFIG->therapy_db().size(), 0);

    monthly_number_of_treatment_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    monthly_number_of_TF_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    monthly_number_of_new_infections_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    monthly_number_of_clinical_episode_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
    monthly_number_of_mutation_events_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    current_number_of_mutation_events_in_this_year_ = 0;
    number_of_mutation_events_by_year_ = LongVector();
  }
}

void ModelDataCollector::perform_population_statistic() {
  //this will do every time the reporter execute the report

  //reset vector
  //    popsize_by_location_.clear();
  //    popsize_by_location_.assign(Model::CONFIG->number_of_locations(), 0);
  //
  //    blood_slide_prevalence_by_location_.clear();
  //    blood_slide_prevalence_by_location_.assign(Model::CONFIG->number_of_locations(), 0.0);
  //
  //    popsize_by_location_hoststate_.clear();
  //    popsize_by_location_hoststate_.assign(Model::CONFIG->number_of_locations(), IntVector(Person::NUMBER_OF_STATE, 0));
  //
  //    total_immune_by_location_.clear();
  //    total_immune_by_location_.assign(Model::CONFIG->number_of_locations(), 0.0);
  //
  //    total_immune_by_location_age_class_.clear();
  //    total_immune_by_location_age_class_.assign(Model::CONFIG->number_of_locations(), DoubleVector(Model::CONFIG->number_of_age_classes(), 0.0));


  for (auto location = 0ul; location < Model::CONFIG->number_of_locations(); location++) {
    popsize_by_location_[location] = 0;
    popsize_residence_by_location_[location] = 0;
    blood_slide_prevalence_by_location_[location] = 0.0;
    fraction_of_positive_that_are_clinical_by_location_[location] = 0.0;
    total_immune_by_location_[location] = 0.0;
    total_parasite_population_by_location_[location] = 0;
    number_of_positive_by_location_[location] = 0;

    for (auto i = 0; i < Person::NUMBER_OF_STATE; i++) {
      popsize_by_location_hoststate_[location][i] = 0;
    }

    for (auto ac = 0ul; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      total_immune_by_location_age_class_[location][ac] = 0.0;
      total_parasite_population_by_location_age_group_[location][ac] = 0;
      number_of_positive_by_location_age_group_[location][ac] = 0;
      number_of_clinical_by_location_age_group_[location][ac] = 0;
      number_of_clinical_by_location_age_group_by_5_[location][ac] = 0;

      popsize_by_location_age_class_[location][ac] = 0;
      popsize_by_location_age_class_by_5_[location][ac] = 0;
      blood_slide_prevalence_by_location_age_group_[location][ac] = 0.0;
      blood_slide_number_by_location_age_group_[location][ac] = 0.0;
      blood_slide_prevalence_by_location_age_group_by_5_[location][ac] = 0.0;
      blood_slide_number_by_location_age_group_by_5_[location][ac] = 0.0;
    }
    for (auto age = 0; age < 80; age++) {
      popsize_by_location_age_[location][age] = 0;
    }

    for (auto i = 0; i < number_of_reported_MOI; i++) {
      multiple_of_infection_by_location_[location][i] = 0;
    }
  }

  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  long long sum_moi = 0;

  for (auto loc = 0ul; loc < Model::CONFIG->number_of_locations(); loc++) {
    auto pop_sum_location = 0;
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (auto ac = 0ul; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        std::size_t size = pi->vPerson()[loc][hs][ac].size();
        popsize_by_location_hoststate_[loc][hs] += (int) size;
        pop_sum_location += size;
        popsize_by_location_age_class_[loc][ac] += size;

        for (int i = 0; i < size; i++) {
          Person* p = pi->vPerson()[loc][hs][ac][i];
          popsize_residence_by_location_[p->residence_location()]++;

          //                    assert(p->has_birthday_event());
          //                    assert(p->age_class() == ac);
          //this immune value will include maternal immunity value of the infants
          double immune_value = p->immune_system()->get_lastest_immune_value();
          total_immune_by_location_[loc] += immune_value;
          total_immune_by_location_age_class_[loc][ac] += immune_value;
          //                    popsize_by_location_age_class_[loc][ac] += 1;
          int ac1 = (p->age() > 70) ? 14 : p->age() / 5;
          popsize_by_location_age_class_by_5_[loc][ac1] += 1;

          if (hs == Person::ASYMPTOMATIC) {
            number_of_positive_by_location_[loc]++;
            number_of_positive_by_location_age_group_[loc][ac] += 1;

            if (p->has_detectable_parasite()) {
              blood_slide_prevalence_by_location_[loc] += 1;
              blood_slide_number_by_location_age_group_[loc][ac] += 1;
              blood_slide_number_by_location_age_group_by_5_[loc][ac1] += 1;
            }
          } else if (hs == Person::CLINICAL) {
            number_of_positive_by_location_[loc]++;
            number_of_positive_by_location_age_group_[loc][ac] += 1;
            blood_slide_prevalence_by_location_[loc] += 1;
            blood_slide_number_by_location_age_group_[loc][ac] += 1;
            blood_slide_number_by_location_age_group_by_5_[loc][ac1] += 1;
            number_of_clinical_by_location_age_group_[loc][ac] += 1;
            number_of_clinical_by_location_age_group_by_5_[loc][ac1] += 1;
          }

          int moi = p->all_clonal_parasite_populations()->size();

          if (moi > 0) {
            sum_moi += moi;

            total_parasite_population_by_location_[loc] += moi;
            total_parasite_population_by_location_age_group_[loc][p->age_class()] += moi;

            if (moi > number_of_reported_MOI) {
              //                            multiple_of_infection_by_location_[loc][number_of_reported_MOI - 1]++;
            } else {
              multiple_of_infection_by_location_[loc][moi - 1]++;
            }
          }

          if (p->age() < 79) {
            popsize_by_location_age_[loc][p->age()] += 1;
          } else {
            popsize_by_location_age_[loc][79] += 1;
          }
        }
      }
    }

    popsize_by_location_[loc] = pop_sum_location;

    const auto sum_popsize_by_location = std::accumulate(
        popsize_by_location_.begin(), popsize_by_location_.end(),
        0
    );
    mean_moi_ = sum_moi / static_cast<double>(sum_popsize_by_location);

    //        double number_of_assymptomatic_and_clinical = blood_slide_prevalence_by_location_[loc] + popsize_by_location_hoststate_[loc][Person::CLINICAL];
    //        number_of_positive_by_location_[loc] = popsize_by_location_hoststate_[loc][Person::ASYMPTOMATIC] + popsize_by_location_hoststate_[loc][Person::CLINICAL];

    //        fraction_of_positive_that_are_clinical_by_location_[loc] = (number_of_positive_by_location_[loc] == 0) ? 0 : ((double) popsize_by_location_hoststate_[loc][Person::CLINICAL]) / number_of_positive_by_location_[loc];
    fraction_of_positive_that_are_clinical_by_location_[loc] = (blood_slide_prevalence_by_location_[loc] == 0)
                                                               ? 0
                                                               : static_cast<double>(popsize_by_location_hoststate_[
            loc][Person::
        CLINICAL]) /
                                                                 blood_slide_prevalence_by_location_[loc];
    const auto number_of_blood_slide_positive = blood_slide_prevalence_by_location_[loc];
    blood_slide_prevalence_by_location_[loc] = blood_slide_prevalence_by_location_[loc] / static_cast<double>(
        pop_sum_location);

    current_EIR_by_location_[loc] =
        (total_number_of_bites_by_location_[loc] - last_update_total_number_of_bites_by_location_[loc]) /
        static_cast<double>(popsize_by_location_[loc]);
    last_update_total_number_of_bites_by_location_[loc] = total_number_of_bites_by_location_[loc];

    last_10_blood_slide_prevalence_by_location_[loc][
        (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency()) %
        10] = blood_slide_prevalence_by_location_[loc];
    last_10_fraction_positive_that_are_clinical_by_location_[loc][
        (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency()) %
        10] = fraction_of_positive_that_are_clinical_by_location_[loc];

    for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      last_10_fraction_positive_that_are_clinical_by_location_age_class_[loc][ac][
          (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency()) %
          10] = (blood_slide_prevalence_by_location_age_group_[loc][ac] == 0)
                ? 0
                : number_of_clinical_by_location_age_group_[loc][ac] /
                  static_cast<double>(blood_slide_prevalence_by_location_age_group_[loc][ac]);
      last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_[loc][ac][
          (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency()) %
          10] = (number_of_blood_slide_positive == 0)
                ? 0
                : number_of_clinical_by_location_age_group_by_5_[loc][ac] /
                  number_of_blood_slide_positive;
      blood_slide_prevalence_by_location_age_group_[loc][ac] =
          blood_slide_number_by_location_age_group_[loc][ac] /
          static_cast<double>(popsize_by_location_age_class_[loc][ac]);
      blood_slide_prevalence_by_location_age_group_by_5_[loc][ac] =
          blood_slide_number_by_location_age_group_by_5_[loc][ac] /
          static_cast<double>(popsize_by_location_age_class_by_5_[loc][ac]);
    }
  }
}

void ModelDataCollector::collect_number_of_bites(const int& location, const int& number_of_bites) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    total_number_of_bites_by_location_[location] += number_of_bites;
    total_number_of_bites_by_location_year_[location] += number_of_bites;
  }
}

void ModelDataCollector::perform_yearly_update() {
  if (Model::SCHEDULER->current_time() == Model::CONFIG->start_collect_data_day()) {
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      person_days_by_location_year_[loc] = Model::POPULATION->size(loc) * Constants::DAYS_IN_YEAR();
    }
  } else if (Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) {
    for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      auto eir = (total_number_of_bites_by_location_year_[loc] /
                  static_cast<double>(person_days_by_location_year_[loc
                  ])) *
                 Constants::DAYS_IN_YEAR();
      //only record year have positive EIR
      //            if (EIR > 0) {
      EIR_by_location_year_[loc].push_back(eir);
      //            }

      //this number will be changed whenever a birth or a death occurs
      // and also when the individual change location
      person_days_by_location_year_[loc] = Model::POPULATION->size(loc) * Constants::DAYS_IN_YEAR();
      total_number_of_bites_by_location_year_[loc] = 0;
      for (auto age = 0; age < 80; age++) {
        number_of_untreated_cases_by_location_age_year_[loc][age] = 0;
        number_of_treatments_by_location_age_year_[loc][age] = 0;
        number_of_deaths_by_location_age_year_[loc][age] = 0;
        number_of_malaria_deaths_by_location_age_year_[loc][age] = 0;

      }
    }
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
      number_of_mutation_events_by_year_.push_back(current_number_of_mutation_events_in_this_year_);
      current_number_of_mutation_events_in_this_year_ = 0;
    }
  }
}

void ModelDataCollector::update_person_days_by_years(const int& location, const int& days) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    person_days_by_location_year_[location] += days;
  }
}

void ModelDataCollector::calculate_eir() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    if (EIR_by_location_year_[loc].empty()) {
      //collect data for less than 1 year
      const auto total_time_in_years =
          (Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) /
          static_cast<double>(Constants::
          DAYS_IN_YEAR());
      double eir = (total_number_of_bites_by_location_year_[loc] /
                    static_cast<double>(person_days_by_location_year_[loc
                    ])) * Constants::DAYS_IN_YEAR();
      eir = eir / total_time_in_years;
      EIR_by_location_[loc] = eir;
    } else {
      double sum_eir = std::accumulate(EIR_by_location_year_[loc].begin(), EIR_by_location_year_[loc].end(), 0.0);
      auto number_of_0 = std::count(EIR_by_location_year_[loc].begin(), EIR_by_location_year_[loc].end(), 0);

      EIR_by_location_[loc] = ((EIR_by_location_year_[loc].size() - number_of_0) == 0.0)
                              ? 0.0
                              : sum_eir /
                                (EIR_by_location_year_[loc].size() -
                                 number_of_0);
    }
  }
}

void ModelDataCollector::collect_1_clinical_episode(const int& location, const int& age, const int& age_class) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    monthly_number_of_clinical_episode_by_location_[location] += 1;
  }

  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
    cumulative_clinical_episodes_by_location_[location]++;

    if (age < 100) {
      cumulative_clinical_episodes_by_location_age_[location][age]++;
    } else {
      cumulative_clinical_episodes_by_location_age_[location][99]++;
    }

    cumulative_clinical_episodes_by_location_age_group_[location][age_class]++;
  }
}

void ModelDataCollector::record_1_death(
    const int& location, const int& birthday, const int& number_of_times_bitten,
    const int& age_group, const int& age
) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    update_person_days_by_years(location, -(Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year()));
    update_average_number_bitten(location, birthday, number_of_times_bitten);
    number_of_death_by_location_age_group_[location][age_group] += 1;
    if (age < 79) {
      number_of_deaths_by_location_age_year_[location][age] += 1;
    } else {
      number_of_deaths_by_location_age_year_[location][79] += 1;
    }
  }
}

void ModelDataCollector::record_1_malaria_death(const int& location, const int& age) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    if (age < 79) {
      number_of_malaria_deaths_by_location_age_year_[location][age] += 1;
    } else {
      number_of_malaria_deaths_by_location_age_year_[location][79] += 1;
    }
  }
}

void ModelDataCollector::update_average_number_bitten(
    const int& location, const int& birthday,
    const int& number_of_times_bitten
) {

  //TODO: check here
  const auto time_living_from_start_collect_data_day = (birthday < Model::CONFIG->start_collect_data_day())
                                                       ? 1
                                                       : Model::SCHEDULER->current_time() + 1 -
                                                         Model::CONFIG->start_collect_data_day();
  const auto average_bites = number_of_times_bitten / static_cast<double>(time_living_from_start_collect_data_day);

  average_number_biten_by_location_person_[location].push_back(average_bites);
}

void ModelDataCollector::calculate_percentage_bites_on_top_20() {
  auto pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto p : pi->vPerson()[loc][hs][ac]) {
          //add to total average number bitten
          update_average_number_bitten(loc, p->birthday(), p->number_of_times_bitten());
        }
      }
    }
  }
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    std::sort(
        average_number_biten_by_location_person_[location].begin(),
        average_number_biten_by_location_person_[location].end(), std::greater<>());
    double total = 0;
    double t20 = 0;
    const auto size20 = static_cast<int>(std::round(
        average_number_biten_by_location_person_[location].size() / 100.0 * 20
    ));
    // TODO: research about size_t and int comparison
    for (auto i = 0; static_cast<size_t>(i) < average_number_biten_by_location_person_[location].size(); i++) {
      total += average_number_biten_by_location_person_[location][i];

      if (i <= size20) {
        t20 += average_number_biten_by_location_person_[location][i];
      }
    }
    percentage_bites_on_top_20_by_location_[location] = (total == 0) ? 0 : t20 / total;
  }
}

void ModelDataCollector::record_1_non_treated_case(const int& location, const int& age) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    if (age <= 79) {
      number_of_untreated_cases_by_location_age_year_[location][age] += 1;
    } else {
      number_of_untreated_cases_by_location_age_year_[location][79] += 1;
    }
  }
}

void ModelDataCollector::begin_time_step() {
  for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    today_number_of_treatments_by_location_[location] = 0;
    today_RITF_by_location_[location] = 0;
    today_TF_by_location_[location] = 0;
  }

  for (auto therapy_id = 0; static_cast<size_t>(therapy_id) < Model::CONFIG->therapy_db().size(); therapy_id++) {
    today_number_of_treatments_by_therapy_[therapy_id] = 0;
    today_tf_by_therapy_[therapy_id] = 0;
  }
}

//TODO: review
void ModelDataCollector::end_of_time_step() {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    double avg_tf = 0;
    for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      total_number_of_treatments_60_by_location_[location][Model::SCHEDULER->current_time() %
                                                           Model::CONFIG->tf_window_size()] = today_number_of_treatments_by_location_[location];
      total_RITF_60_by_location_[location][Model::SCHEDULER->current_time() %
                                           Model::CONFIG->tf_window_size()] = today_RITF_by_location_[location];
      total_TF_60_by_location_[location][Model::SCHEDULER->current_time() %
                                         Model::CONFIG->tf_window_size()] = today_TF_by_location_[location];

      auto t_treatment60 = 0;
      auto t_ritf60 = 0;
      auto t_tf60 = 0;
      for (auto i = 0; i < Model::CONFIG->tf_window_size(); i++) {
        t_treatment60 += total_number_of_treatments_60_by_location_[location][i];
        t_ritf60 += total_RITF_60_by_location_[location][i];
        t_tf60 += total_TF_60_by_location_[location][i];
      }
      current_RITF_by_location_[location] = (t_treatment60 == 0) ? 0 : static_cast<double>(t_ritf60) /
                                                                       t_treatment60;
      current_TF_by_location_[location] = (t_treatment60 == 0) ? 0 : static_cast<double>(t_tf60) / t_treatment60;

      current_TF_by_location_[location] =
          (current_TF_by_location_[location]) > 1 ? 1 : current_TF_by_location_[location];
      current_RITF_by_location_[location] =
          (current_RITF_by_location_[location]) > 1 ? 1 : current_RITF_by_location_[location];

      avg_tf += current_TF_by_location_[location];
    }

    //update UTL
    if ((avg_tf / static_cast<double>(Model::CONFIG->number_of_locations())) <= Model::CONFIG->tf_rate()) {
      current_utl_duration_ += 1;
    }
    for (auto therapy_id = 0; static_cast<size_t>(therapy_id) < Model::CONFIG->therapy_db().size(); therapy_id++) {
      total_number_of_treatments_60_by_therapy_[therapy_id][Model::SCHEDULER->current_time() %
                                                            Model::CONFIG->tf_window_size()] = today_number_of_treatments_by_therapy_[therapy_id];
      total_tf_60_by_therapy_[therapy_id][Model::SCHEDULER->current_time() %
                                          Model::CONFIG->tf_window_size()] = today_tf_by_therapy_[therapy_id];

      auto t_treatment60 = 0;
      auto t_tf60 = 0;
      for (auto i = 0; i < Model::CONFIG->tf_window_size(); i++) {
        t_treatment60 += total_number_of_treatments_60_by_therapy_[therapy_id][i];
        t_tf60 += total_tf_60_by_therapy_[therapy_id][i];
      }

      current_tf_by_therapy_[therapy_id] = (t_treatment60 == 0) ? 0 : static_cast<double>(t_tf60) / t_treatment60;
      current_tf_by_therapy_[therapy_id] =
          (current_tf_by_therapy_[therapy_id]) > 1 ? 1 : current_tf_by_therapy_[therapy_id];
    }
  }


}

void ModelDataCollector::record_1_treatment(const int& location, const int& age, const int& therapy_id) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    today_number_of_treatments_by_location_[location] += 1;
    today_number_of_treatments_by_therapy_[therapy_id] += 1;

    number_of_treatments_with_therapy_ID_[therapy_id] += 1;

    monthly_number_of_treatment_by_location_[location] += 1;

    if (age <= 79) {
      number_of_treatments_by_location_age_year_[location][age] += 1;
    } else {
      number_of_treatments_by_location_age_year_[location][79] += 1;
    }
  }
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
    cumulative_number_treatments_by_location_[location] += 1;
  }
}

void ModelDataCollector::record_1_mutation(const int& location, Genotype* from, Genotype* to) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    cumulative_mutants_by_location_[location] += 1;
    monthly_number_of_mutation_events_by_location_[location] += 1;
  }
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
    current_number_of_mutation_events_in_this_year_ += 1;
  }
}

void ModelDataCollector::update_UTL_vector() {
  UTL_duration_.push_back(current_utl_duration_);
  current_utl_duration_ = 0;
}

//void ModelDataCollector::collect_1_non_resistant_treatment(const int& therapy_id) {
//    number_of_treatments_with_therapy_ID_[therapy_id] += 1;
//}

void ModelDataCollector::record_1_TF(const int& location, const bool& by_drug) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    //if treatment failure due to drug (both resistance or not clear)
    if (by_drug) {
      today_TF_by_location_[location] += 1;
      monthly_number_of_TF_by_location_[location] += 1;
    }
  }

  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
    const auto current_discounted_tf = exp(
        log(0.97) * floor((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) /
                          Constants::DAYS_IN_YEAR()));

    cumulative_discounted_NTF_by_location_[location] += current_discounted_tf;
    cumulative_NTF_by_location_[location] += 1;
    if (by_drug) {
      cumulative_TF_by_location_[location] += 1;
    }
  }
}

void
ModelDataCollector::record_1_treatment_failure_by_therapy(const int& location, const int& age, const int& therapy_id) {
  number_of_treatments_fail_with_therapy_ID_[therapy_id] += 1;
  today_tf_by_therapy_[therapy_id] += 1;
}

void ModelDataCollector::record_1_treatment_success_by_therapy(const int& therapy_id) {
  number_of_treatments_success_with_therapy_ID_[therapy_id] += 1;
}

void ModelDataCollector::update_after_run() {
  perform_population_statistic();

  calculate_eir();
  calculate_percentage_bites_on_top_20();

  UTL_duration_.push_back(current_utl_duration_);

  current_utl_duration_ = 0;
  for (int utl : UTL_duration_) {
    current_utl_duration_ += utl;
  }
}

void ModelDataCollector::record_AMU_AFU(
    Person* person, Therapy* therapy,
    ClonalParasitePopulation* clinical_caused_parasite
) {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_of_comparison_period()) {
    auto sc_therapy = dynamic_cast<SCTherapy*>(therapy);
    if (sc_therapy != nullptr) {
      const auto art_id = sc_therapy->get_arteminsinin_id();
      if (art_id != -1 && sc_therapy->drug_ids.size() > 1) {
        const auto number_of_drugs_in_therapy = sc_therapy->drug_ids.size();
        const auto discounted_fraction = exp(
            log(0.97) *
            floor((Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) /
                  Constants::DAYS_IN_YEAR()));
        //            assert(false);
        //combine therapy
        for (auto i = 0; i < number_of_drugs_in_therapy; i++) {
          int drug_id = sc_therapy->drug_ids[i];
          if (drug_id != art_id) {
            //only check for the remaining chemical drug != artemisinin
            const auto parasite_population_size = person->all_clonal_parasite_populations()->size();

            auto found_amu = false;
            auto found_afu = false;
            for (auto j = 0ul; j < parasite_population_size; j++) {
              ClonalParasitePopulation* bp = person->all_clonal_parasite_populations()->parasites()->at(j);
              if (bp->resist_to(drug_id) && !bp->resist_to(art_id)) {
                found_amu = true;
                AMU_per_parasite_pop_ +=
                    sc_therapy->get_max_dosing_day() / static_cast<double>(parasite_population_size);
                discounted_AMU_per_parasite_pop_ +=
                    discounted_fraction * sc_therapy->get_max_dosing_day() /
                    static_cast<double>(parasite_population_size);
                if (bp == clinical_caused_parasite) {
                  AMU_for_clinical_caused_parasite_ += sc_therapy->get_max_dosing_day();
                  discounted_AMU_for_clinical_caused_parasite_ +=
                      discounted_fraction * sc_therapy->get_max_dosing_day();
                }
              }

              if (bp->resist_to(drug_id) && bp->resist_to(art_id)) {
                found_afu = true;
              }
            }
            if (found_amu) {
              AMU_per_person_ += sc_therapy->get_max_dosing_day();
              discounted_AMU_per_person_ += discounted_fraction * sc_therapy->get_max_dosing_day();
            }

            if (found_afu) {
              AFU_ += sc_therapy->get_max_dosing_day();
              discounted_AFU_ += discounted_fraction * sc_therapy->get_max_dosing_day();
            }
          }
        }
      }
    }
  }
}

double ModelDataCollector::get_blood_slide_prevalence(const int& location, const int& age_from, const int& age_to) {
  double blood_slide_numbers = 0;
  double popsize = 0;
  //    age count from 0

  if (age_from < 10) {
    if (age_to <= 10) {
      for (int ac = age_from; ac <= age_to; ac++) {
        blood_slide_numbers += blood_slide_number_by_location_age_group_[location][ac];
        popsize += popsize_by_location_age_class_[location][ac];
      }
    } else {
      for (int ac = age_from; ac <= 10; ac++) {
        blood_slide_numbers += blood_slide_number_by_location_age_group_[location][ac];
        popsize += popsize_by_location_age_class_[location][ac];
      }
      int ac = 10;
      while (ac < age_to) {
        blood_slide_numbers += blood_slide_number_by_location_age_group_by_5_[location][ac / 5];
        popsize += popsize_by_location_age_class_by_5_[location][ac / 5];
        ac += 5;
      }
    }
  } else {
    int ac = age_from;

    while (ac < age_to) {
      blood_slide_numbers += blood_slide_number_by_location_age_group_by_5_[location][ac / 5];
      popsize += popsize_by_location_age_class_by_5_[location][ac / 5];
      ac += 5;
    }
  }
  return (popsize == 0) ? 0 : blood_slide_numbers / popsize;
}

void ModelDataCollector::monthly_update() {
  if (Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) {
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      monthly_number_of_treatment_by_location_[loc] = 0;
      monthly_number_of_TF_by_location_[loc] = 0;
      monthly_number_of_new_infections_by_location_[loc] = 0;
      monthly_number_of_clinical_episode_by_location_[loc] = 0;
      monthly_number_of_mutation_events_by_location_[loc] = 0;
    }
  }
}

void ModelDataCollector::record_1_migration(Person* pPerson, const int& from, const int& to) {

}
