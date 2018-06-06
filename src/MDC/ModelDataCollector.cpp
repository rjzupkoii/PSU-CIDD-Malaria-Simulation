/* 
 * File:   Statistic.cpp
 * Author: Merlin
 * 
 * Created on July 9, 2013, 2:28 PM
 */

#include "ModelDataCollector.h"
#include "../Model.h"
#include "../Config.h"
#include "../Strategies/IStrategy.h"
#include "../Person.h"
#include "../PersonIndexByLocationStateAgeClass.h"
#include "../Population.h"
#include "../ImmuneSystem.h"
#include "../SingleHostClonalParasitePopulations.h"
#include "../SCTherapy.h"
#include "../ClonalParasitePopulation.h"
#include <numeric>

ModelDataCollector::ModelDataCollector(Model *model) : model_(model), current_utl_duration_(),
                                                       popsize_by_location_(), blood_slide_prevalence_by_location_(),
                                                       fraction_of_positive_that_are_clinical_by_location_(),
                                                       popsize_by_location_hoststate_(), total_immune_by_location_(),
                                                       total_immune_by_location_age_class_(),
                                                       total_number_of_bites_by_location_(),
                                                       total_number_of_bites_by_location_year_(),
                                                       person_days_by_location_year_(), EIR_by_location_year_(),
                                                       EIR_by_location_(),
                                                       cumulative_clinical_episodes_by_location_(),
                                                       cumulative_clinical_episodes_by_location_age_(),
                                                       popsize_residence_by_location_(), resistance_tracker_() {
}

ModelDataCollector::~ModelDataCollector() {
}

void ModelDataCollector::initialize() {
    if (model_ != NULL) {
        popsize_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
        popsize_residence_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

        blood_slide_prevalence_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        blood_slide_prevalence_by_location_age_group_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                      DoubleVector(
                                                                              Model::CONFIG->number_of_age_classes(),
                                                                              0.0));
        blood_slide_number_by_location_age_group_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                  DoubleVector(Model::CONFIG->number_of_age_classes(),
                                                                               0.0));
        blood_slide_prevalence_by_location_age_group_by_5_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                           DoubleVector(
                                                                                   Model::CONFIG->number_of_age_classes(),
                                                                                   0.0));
        blood_slide_number_by_location_age_group_by_5_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                       DoubleVector(
                                                                               Model::CONFIG->number_of_age_classes(),
                                                                               0.0));
        fraction_of_positive_that_are_clinical_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        popsize_by_location_hoststate_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                    IntVector(Person::NUMBER_OF_STATE, 0));
        popsize_by_location_age_class_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                    IntVector(Model::CONFIG->number_of_age_classes(), 0));
        popsize_by_location_age_class_by_5_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                         IntVector(Model::CONFIG->number_of_age_classes(), 0));

        total_immune_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        total_immune_by_location_age_class_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                            DoubleVector(Model::CONFIG->number_of_age_classes(), 0.0));

        total_number_of_bites_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0);
        total_number_of_bites_by_location_year_ = LongVector(Model::CONFIG->number_of_locations(), 0);
        person_days_by_location_year_ = LongVector(Model::CONFIG->number_of_locations(), 0);

        EIR_by_location_year_ = DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
        EIR_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

        cumulative_clinical_episodes_by_location_ = LongVector(Model::CONFIG->number_of_locations(), 0);
        cumulative_clinical_episodes_by_location_age_ = LongVector2(Model::CONFIG->number_of_locations(),
                                                                    LongVector(100, 0));
        cumulative_clinical_episodes_by_location_age_group_ = LongVector2(Model::CONFIG->number_of_locations(),
                                                                          LongVector(
                                                                                  Model::CONFIG->number_of_age_classes(),
                                                                                  0));

        average_number_biten_by_location_person_ = DoubleVector2(Model::CONFIG->number_of_locations(), DoubleVector());
        percentage_bites_on_top_20_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

        cumulative_discounted_NTF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        cumulative_NTF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

        today_TF_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0.0);
        today_number_of_treatments_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0.0);
        today_RITF_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0.0);

        total_number_of_treatments_60_by_location_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                                IntVector(Model::CONFIG->tf_window_size(), 0));
        total_RITF_60_by_location_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                IntVector(Model::CONFIG->tf_window_size(), 0));
        total_TF_60_by_location_ = IntVector2(Model::CONFIG->number_of_locations(),
                                              IntVector(Model::CONFIG->tf_window_size(), 0));

        current_RITF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        current_TF_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);

        cumulative_mutants_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0.0);

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

        multiple_of_infection_by_location_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                        IntVector(number_of_reported_MOI, 0));

        current_EIR_by_location_ = DoubleVector(Model::CONFIG->number_of_locations(), 0.0);
        last_update_total_number_of_bites_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0.0);

        resistance_tracker_.initialize();

        last_10_blood_slide_prevalence_by_location_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                    DoubleVector(10, 0.0));
        last_10_fraction_positive_that_are_clinical_by_location_ = DoubleVector2(Model::CONFIG->number_of_locations(),
                                                                                 DoubleVector(10, 0.0));
        last_10_fraction_positive_that_are_clinical_by_location_age_class_ = DoubleVector3(
                Model::CONFIG->number_of_locations(),
                DoubleVector2(Model::CONFIG->number_of_age_classes(), DoubleVector(10, 0.0)));
        last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_ = DoubleVector3(
                Model::CONFIG->number_of_locations(),
                DoubleVector2(Model::CONFIG->number_of_age_classes(), DoubleVector(10, 0.0)));
        total_parasite_population_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
        number_of_positive_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

        total_parasite_population_by_location_age_group_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                                      IntVector(Model::CONFIG->number_of_age_classes(),
                                                                                0));
        number_of_positive_by_location_age_group_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                               IntVector(Model::CONFIG->number_of_age_classes(), 0));
        number_of_clinical_by_location_age_group_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                               IntVector(Model::CONFIG->number_of_age_classes(), 0));
        number_of_clinical_by_location_age_group_by_5_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                                    IntVector(Model::CONFIG->number_of_age_classes(),
                                                                              0));
        number_of_death_by_location_age_group_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                            IntVector(Model::CONFIG->number_of_age_classes(), 0));

        number_of_untreated_cases_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                                     IntVector(80, 0));
        number_of_treatments_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));
        number_of_deaths_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));
        number_of_malaria_deaths_by_location_age_year_ = IntVector2(Model::CONFIG->number_of_locations(),
                                                                    IntVector(80, 0));
        number_of_treatments_by_location_age_therapy_year_ = IntVector3(Model::CONFIG->number_of_locations(),
                                                                        IntVector2(80, IntVector(3, 0)));
        number_of_treatment_failures_by_location_age_therapy_year_ = IntVector3(Model::CONFIG->number_of_locations(),
                                                                                IntVector2(80, IntVector(3, 0)));
        popsize_by_location_age_ = IntVector2(Model::CONFIG->number_of_locations(), IntVector(80, 0));

        TF_at_15_ = 0;
        single_resistance_frequency_at_15_ = 0;
        double_resistance_frequency_at_15_ = 0;
        triple_resistance_frequency_at_15_ = 0;
        quadruple_resistance_frequency_at_15_ = 0;
        quintuple_resistance_frequency_at_15_ = 0;
        art_resistance_frequency_at_15_ = 0;
        total_resistance_frequency_at_15_ = 0;


        total_number_of_treatments_60_by_therapy_ = IntVector2(Model::CONFIG->therapy_db().size(),
                                                               IntVector(Model::CONFIG->tf_window_size(), 0));
        total_TF_60_by_therapy_ = IntVector2(Model::CONFIG->therapy_db().size(),
                                             IntVector(Model::CONFIG->tf_window_size(), 0));
        current_TF_by_therapy_ = DoubleVector(Model::CONFIG->therapy_db().size(), 0.0);
        today_TF_by_therapy_ = IntVector(Model::CONFIG->therapy_db().size(), 0);
        today_number_of_treatments_by_therapy_ = IntVector(Model::CONFIG->therapy_db().size(), 0);

        monthly_number_of_treatment_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);
        monthly_number_of_clinical_episode_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

    }
}

void ModelDataCollector::perform_population_statistic() {
    acc = mean_acc();
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




    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        popsize_by_location_[location] = 0;
        popsize_residence_by_location_[location] = 0;
        blood_slide_prevalence_by_location_[location] = 0.0;
        fraction_of_positive_that_are_clinical_by_location_[location] = 0.0;
        total_immune_by_location_[location] = 0.0;
        total_parasite_population_by_location_[location] = 0;
        number_of_positive_by_location_[location] = 0;

        for (int i = 0; i < Person::NUMBER_OF_STATE; i++) {
            popsize_by_location_hoststate_[location][i] = 0;
        }

        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
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
        for (int age = 0; age < 80; age++) {
            popsize_by_location_age_[location][age] = 0;

        }


        for (int i = 0; i < number_of_reported_MOI; i++) {
            multiple_of_infection_by_location_[location][i] = 0;
        }

    }

    auto *pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        int pop_sum_location = 0;
        for (int hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                int size = pi->vPerson()[loc][hs][ac].size();
                popsize_by_location_hoststate_[loc][hs] += size;
                pop_sum_location += size;
                popsize_by_location_age_class_[loc][ac] += size;

                for (int i = 0; i < size; i++) {
                    Person *p = pi->vPerson()[loc][hs][ac][i];
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
                        acc(moi);

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

        //        double number_of_assymptomatic_and_clinical = blood_slide_prevalence_by_location_[loc] + popsize_by_location_hoststate_[loc][Person::CLINICAL];
        //        number_of_positive_by_location_[loc] = popsize_by_location_hoststate_[loc][Person::ASYMPTOMATIC] + popsize_by_location_hoststate_[loc][Person::CLINICAL];

        //        fraction_of_positive_that_are_clinical_by_location_[loc] = (number_of_positive_by_location_[loc] == 0) ? 0 : ((double) popsize_by_location_hoststate_[loc][Person::CLINICAL]) / number_of_positive_by_location_[loc];
        fraction_of_positive_that_are_clinical_by_location_[loc] = (blood_slide_prevalence_by_location_[loc] == 0) ? 0 :
                                                                   ((double) popsize_by_location_hoststate_[loc][Person::CLINICAL]) /
                                                                   blood_slide_prevalence_by_location_[loc];
        double number_of_blood_slide_positive = blood_slide_prevalence_by_location_[loc];
        blood_slide_prevalence_by_location_[loc] = blood_slide_prevalence_by_location_[loc] / (double) pop_sum_location;


        current_EIR_by_location_[loc] =
                (total_number_of_bites_by_location_[loc] - last_update_total_number_of_bites_by_location_[loc]) /
                (double) popsize_by_location_[loc];
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
                    10] = (blood_slide_prevalence_by_location_age_group_[loc][ac] == 0) ? 0 :
                          number_of_clinical_by_location_age_group_[loc][ac] /
                          (double) blood_slide_prevalence_by_location_age_group_[loc][ac];
            last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5_[loc][ac][
                    (Model::SCHEDULER->current_time() / Model::CONFIG->report_frequency()) %
                    10] = (number_of_blood_slide_positive == 0) ? 0 :
                          number_of_clinical_by_location_age_group_by_5_[loc][ac] /
                          (double) number_of_blood_slide_positive;
            blood_slide_prevalence_by_location_age_group_[loc][ac] =
                    blood_slide_number_by_location_age_group_[loc][ac] /
                    (double) popsize_by_location_age_class_[loc][ac];
            blood_slide_prevalence_by_location_age_group_by_5_[loc][ac] =
                    blood_slide_number_by_location_age_group_by_5_[loc][ac] /
                    (double) popsize_by_location_age_class_by_5_[loc][ac];
        }
    }
}

void ModelDataCollector::collect_number_of_bites(const int &location, const int &number_of_bites) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        total_number_of_bites_by_location_[location] += number_of_bites;
        total_number_of_bites_by_location_year_[location] += number_of_bites;
    }
}

void ModelDataCollector::perform_yearly_update() {
    if (Model::SCHEDULER->current_time() == Model::CONFIG->start_collect_data_day()) {
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            person_days_by_location_year_[loc] = Model::POPULATION->size(loc) * 360;
        }
    } else if ((Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) &&
               (((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) % 360) == 0)) {

        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {

            double EIR = (total_number_of_bites_by_location_year_[loc] / (double) person_days_by_location_year_[loc]) *
                         360.0;
            //only record year have positive EIR
            //            if (EIR > 0) {
            EIR_by_location_year_[loc].push_back(EIR);
            //            }

            //this number will be changed whenever a birth or a death occurs
            // and also when the individual change location
            person_days_by_location_year_[loc] = Model::POPULATION->size(loc) * 360;
            total_number_of_bites_by_location_year_[loc] = 0;
            for (int age = 0; age < 80; age++) {
                number_of_untreated_cases_by_location_age_year_[loc][age] = 0;
                number_of_treatments_by_location_age_year_[loc][age] = 0;
                number_of_deaths_by_location_age_year_[loc][age] = 0;
                number_of_malaria_deaths_by_location_age_year_[loc][age] = 0;
                for (int therapy_id = 0; therapy_id < 3; therapy_id++) {
                    number_of_treatments_by_location_age_therapy_year_[loc][age][therapy_id] = 0;
                    number_of_treatment_failures_by_location_age_therapy_year_[loc][age][therapy_id] = 0;
                }
            }
        }
    }
}

void ModelDataCollector::update_person_days_by_years(const int &location, const int &days) {

    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        person_days_by_location_year_[location] += days;
    }
}

void ModelDataCollector::calculate_EIR() {
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        if (EIR_by_location_year_[loc].size() == 0) {
            //collect data for less than 1 year            
            double total_time_in_years =
                    (Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) / 360.0;
            double EIR = (total_number_of_bites_by_location_year_[loc] / (double) person_days_by_location_year_[loc]) *
                         360.0;
            EIR = EIR / total_time_in_years;
            EIR_by_location_[loc] = EIR;
        } else {
            double sum_EIR = std::accumulate(EIR_by_location_year_[loc].begin(), EIR_by_location_year_[loc].end(), 0.0);
            int number_of_0 = std::count(EIR_by_location_year_[loc].begin(), EIR_by_location_year_[loc].end(), 0);

            EIR_by_location_[loc] = ((EIR_by_location_year_[loc].size() - number_of_0) == 0.0) ? 0.0 : sum_EIR /
                                                                                                       (EIR_by_location_year_[loc].size() -
                                                                                                        number_of_0);
        }


    }
}

void ModelDataCollector::collect_1_clinical_episode(const int &location, const int &age, const int &age_class) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        cumulative_clinical_episodes_by_location_[location]++;
        monthly_number_of_clinical_episode_by_location_[location] += 1;

        if (age < 100) {
            cumulative_clinical_episodes_by_location_age_[location][age]++;
        } else {
            cumulative_clinical_episodes_by_location_age_[location][99]++;
        }

        cumulative_clinical_episodes_by_location_age_group_[location][age_class]++;
    }
}

void ModelDataCollector::record_1_death(const int &location, const int &birthday, const int &number_of_times_bitten,
                                        const int &age_group, const int &age) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        update_person_days_by_years(location, -(360 - Model::SCHEDULER->current_day_in_year()));
        update_average_number_bitten(location, birthday, number_of_times_bitten);
        number_of_death_by_location_age_group_[location][age_group] += 1;
        if (age < 79) {
            number_of_deaths_by_location_age_year_[location][age] += 1;
        } else {
            number_of_deaths_by_location_age_year_[location][79] += 1;
        }

    }
}

void ModelDataCollector::record_1_malaria_death(const int &location, const int &age) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        if (age < 79) {
            number_of_malaria_deaths_by_location_age_year_[location][age] += 1;
        } else {
            number_of_malaria_deaths_by_location_age_year_[location][79] += 1;
        }

    }
}

void ModelDataCollector::update_average_number_bitten(const int &location, const int &birthday,
                                                      const int &number_of_times_bitten) {
    int time_living_from_start_collect_data_day = (birthday < Model::CONFIG->start_collect_data_day()) ? 1 :
                                                  Model::SCHEDULER->current_time() + 1 -
                                                  Model::CONFIG->start_collect_data_day();
    double average_bites = number_of_times_bitten / (double) time_living_from_start_collect_data_day;

    average_number_biten_by_location_person_[location].push_back(average_bites);
}

void ModelDataCollector::calculate_percentage_bites_on_top_20() {
    PersonIndexByLocationStateAgeClass *pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                int size = pi->vPerson()[loc][hs][ac].size();
                for (int i = 0; i < size; i++) {
                    Person *p = pi->vPerson()[loc][hs][ac][i];
                    //add to total average number bitten
                    update_average_number_bitten(loc, p->birthday(), p->number_of_times_bitten());
                }
            }
        }
    }
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        std::sort(average_number_biten_by_location_person_[location].begin(),
                  average_number_biten_by_location_person_[location].end(), std::greater<double>());
        double total = 0;
        double t20 = 0;
        int size20 = average_number_biten_by_location_person_[location].size() / 100.0 * 20;
        for (int i = 0; i < average_number_biten_by_location_person_[location].size(); i++) {
            total += average_number_biten_by_location_person_[location][i];

            if (i <= size20) {
                t20 += average_number_biten_by_location_person_[location][i];
            }
        }
        percentage_bites_on_top_20_by_location_[location] = (total == 0) ? 0 : t20 / total;
    }

}

void ModelDataCollector::record_1_non_treated_case(const int &location, const int &age) {
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

    for (int therapy_id = 0; therapy_id < Model::CONFIG->therapy_db().size(); therapy_id++) {
        today_number_of_treatments_by_therapy_[therapy_id] = 0;
        today_TF_by_therapy_[therapy_id] = 0;
    }

}

void ModelDataCollector::end_of_time_step() {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {

        double avg_TF = 0;
        for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
            total_number_of_treatments_60_by_location_[location][Model::SCHEDULER->current_time() %
                                                                 Model::CONFIG->tf_window_size()] = today_number_of_treatments_by_location_[location];
            total_RITF_60_by_location_[location][Model::SCHEDULER->current_time() %
                                                 Model::CONFIG->tf_window_size()] = today_RITF_by_location_[location];
            total_TF_60_by_location_[location][Model::SCHEDULER->current_time() %
                                               Model::CONFIG->tf_window_size()] = today_TF_by_location_[location];

            int tTreatment60 = 0;
            int tRITF60 = 0;
            int tTF60 = 0;
            for (int i = 0; i < Model::CONFIG->tf_window_size(); i++) {
                tTreatment60 += total_number_of_treatments_60_by_location_[location][i];
                tRITF60 += total_RITF_60_by_location_[location][i];
                tTF60 += total_TF_60_by_location_[location][i];
            }
            current_RITF_by_location_[location] = (tTreatment60 == 0) ? 0 : (double) tRITF60 / (double) tTreatment60;
            current_TF_by_location_[location] = (tTreatment60 == 0) ? 0 : (double) tTF60 / (double) tTreatment60;

            current_TF_by_location_[location] =
                    (current_TF_by_location_[location]) > 1 ? 1 : current_TF_by_location_[location];
            current_RITF_by_location_[location] =
                    (current_RITF_by_location_[location]) > 1 ? 1 : current_RITF_by_location_[location];

            avg_TF += current_TF_by_location_[location];
        }

        //update UTL
        if (avg_TF / (double) Model::CONFIG->number_of_locations() <= Model::CONFIG->TF_rate()) {
            current_utl_duration_ += 1;
        }
        for (int therapy_id = 0; therapy_id < Model::CONFIG->therapy_db().size(); therapy_id++) {
            total_number_of_treatments_60_by_therapy_[therapy_id][Model::SCHEDULER->current_time() %
                                                                  Model::CONFIG->tf_window_size()] = today_number_of_treatments_by_therapy_[therapy_id];
            total_TF_60_by_therapy_[therapy_id][Model::SCHEDULER->current_time() %
                                                Model::CONFIG->tf_window_size()] = today_TF_by_therapy_[therapy_id];

            int tTreatment60 = 0;
            int tTF60 = 0;
            for (int i = 0; i < Model::CONFIG->tf_window_size(); i++) {
                tTreatment60 += total_number_of_treatments_60_by_therapy_[therapy_id][i];
                tTF60 += total_TF_60_by_therapy_[therapy_id][i];
            }

            current_TF_by_therapy_[therapy_id] = (tTreatment60 == 0) ? 0 : (double) tTF60 / (double) tTreatment60;
            current_TF_by_therapy_[therapy_id] =
                    (current_TF_by_therapy_[therapy_id]) > 1 ? 1 : current_TF_by_therapy_[therapy_id];
        }

    }

    resistance_tracker_.update_resistance_tracker();

}

void ModelDataCollector::record_1_treatment(const int &location, const int &age, const int &therapy_id) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        today_number_of_treatments_by_location_[location] += 1;
        today_number_of_treatments_by_therapy_[therapy_id] += 1;

        number_of_treatments_with_therapy_ID_[therapy_id] += 1;

        monthly_number_of_treatment_by_location_[location] += 1;

        if (age <= 79) {
            number_of_treatments_by_location_age_year_[location][age] += 1;
            if (therapy_id < 3) {
                number_of_treatments_by_location_age_therapy_year_[location][age][therapy_id] += 1;
            }
        } else {
            number_of_treatments_by_location_age_year_[location][79] += 1;
            if (therapy_id < 3) {
                number_of_treatments_by_location_age_therapy_year_[location][79][therapy_id] += 1;
            }
        }

    }
}

void ModelDataCollector::record_1_mutation(const int &location, IntGenotype *from, IntGenotype *to) {

    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        cumulative_mutants_by_location_[location] += 1;
    }

    resistance_tracker_.change(from->genotype_id(), to->genotype_id(), location);
}

void ModelDataCollector::update_UTL_vector() {
    UTL_duration_.push_back(current_utl_duration_);
    current_utl_duration_ = 0;
}

//void ModelDataCollector::collect_1_non_resistant_treatment(const int& therapy_id) {
//    number_of_treatments_with_therapy_ID_[therapy_id] += 1;
//}

void ModelDataCollector::record_1_TF(const int &location, const bool &by_drug) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {

        //if treatment failure due to drug (both resistance or not clear)
        if (by_drug) {
            today_TF_by_location_[location] += 1;
        }
    }

    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_intervention_day()) {
        double current_discounted_tf = exp(
                log(0.97) * floor((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) / 360));

        cumulative_discounted_NTF_by_location_[location] += current_discounted_tf;
        cumulative_NTF_by_location_[location] += 1;
    }

}

void
ModelDataCollector::record_1_treatment_failure_by_therapy(const int &location, const int &age, const int &therapy_id) {
    number_of_treatments_fail_with_therapy_ID_[therapy_id] += 1;
    today_TF_by_therapy_[therapy_id] += 1;

    if (therapy_id < 3) {
        if (age < 79) {
            number_of_treatment_failures_by_location_age_therapy_year_[location][age][therapy_id] += 1;
        } else {
            number_of_treatment_failures_by_location_age_therapy_year_[location][79][therapy_id] += 1;
        }
    }
}

void ModelDataCollector::record_1_treatment_success_by_therapy(const int &therapy_id) {
    number_of_treatments_success_with_therapy_ID_[therapy_id] += 1;
}

void ModelDataCollector::update_after_run() {
    perform_population_statistic();


    calculate_EIR();
    calculate_percentage_bites_on_top_20();

    UTL_duration_.push_back(current_utl_duration_);

    current_utl_duration_ = 0;
    for (int i = 0; i < UTL_duration_.size(); i++) {
        current_utl_duration_ += UTL_duration_[i];
    }

}

void ModelDataCollector::record_1_RITF(const int &location) {
    if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
        today_RITF_by_location_[location] += 1;
    }
}

void ModelDataCollector::record_AMU_AFU(Person *person, Therapy *therapy,
                                        ClonalParasitePopulation *clinical_caused_parasite) {
    SCTherapy *scTherapy = dynamic_cast<SCTherapy *> (therapy);
    if (scTherapy != NULL) {
        int artId = scTherapy->get_arteminsinin_id();
        if (artId != -1 && scTherapy->drug_ids().size() > 1) {
            int numberOfDrugsInTherapy = scTherapy->drug_ids().size();
            double discouted_fraction = exp(
                    log(0.97) * floor((Model::SCHEDULER->current_time() - Model::CONFIG->start_treatment_day()) / 360));
            //            assert(false);
            //combine therapy
            for (int i = 0; i < numberOfDrugsInTherapy; i++) {
                int drugId = scTherapy->drug_ids()[i];
                if (drugId != artId) {
                    //only check for the remaining chemical drug != artemisinin
                    int parasitePopulationSize = person->all_clonal_parasite_populations()->size();

                    bool foundAMU = false;
                    bool foundAFU = false;
                    for (int j = 0; j < parasitePopulationSize; j++) {
                        ClonalParasitePopulation *bp = person->all_clonal_parasite_populations()->parasites()->at(j);
                        if (bp->resist_to(drugId) && !bp->resist_to(artId)) {
                            foundAMU = true;
                            AMU_per_parasite_pop_ += scTherapy->dosing_day() / (double) parasitePopulationSize;
                            discounted_AMU_per_parasite_pop_ +=
                                    discouted_fraction * scTherapy->dosing_day() / (double) parasitePopulationSize;
                            if (bp == clinical_caused_parasite) {
                                AMU_for_clinical_caused_parasite_ += scTherapy->dosing_day();
                                discounted_AMU_for_clinical_caused_parasite_ +=
                                        discouted_fraction * scTherapy->dosing_day();
                            }
                        }

                        if (bp->resist_to(drugId) && bp->resist_to(artId)) {
                            foundAFU = true;
                        }
                    }
                    if (foundAMU) {
                        AMU_per_person_ += scTherapy->dosing_day();
                        discounted_AMU_per_person_ += discouted_fraction * scTherapy->dosing_day();
                    }

                    if (foundAFU) {
                        AFU_ += scTherapy->dosing_day();
                        discounted_AFU_ += discouted_fraction * scTherapy->dosing_day();
                    }
                }
            }
        }
    }
}

double ModelDataCollector::get_blood_slide_prevalence(const int &location, const int &age_from, const int &age_to) {
    double blood_slide_numbers = 0;
    double popsize = 0;
    //    age count from 0

    if (age_from < 10) {

        if (age_to <= 10) {
            for (int ac = age_from; ac < age_to; ac++) {
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
    return 0;
}

void ModelDataCollector::perform_monthly_update() {
    if ((Model::SCHEDULER->current_time() > Model::CONFIG->start_collect_data_day()) &&
        (((Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) % 30) == 0)) {
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            monthly_number_of_treatment_by_location_[loc] = 0;
            monthly_number_of_clinical_episode_by_location_[loc] = 0;
        }

    }
}

void ModelDataCollector::record_1_migration(Person *pPerson, const int &from, const int &to) {
    for (auto clonal : *(pPerson->all_clonal_parasite_populations()->parasites())) {
        resistance_tracker_.change_location(clonal->genotype()->genotype_id(), from, to );
    }
}
