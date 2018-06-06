/* 
 * File:   Statistic.h
 * Author: Merlin
 *
 * Created on July 9, 2013, 2:28 PM
 */

#ifndef MODELDATACOLLECTOR_H
#define MODELDATACOLLECTOR_H

#include "../PropertyMacro.h"
#include "../TypeDef.h"
#include "ResistanceTracker.h"
#include <boost/version.hpp>

#if BOOST_VERSION >= 106400  // or 64, need to check

#include <boost/serialization/array_wrapper.hpp>

#endif

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

class Model;

class IntGenotype;

class Person;

class Therapy;

class ClonalParasitePopulation;

typedef boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean> > mean_acc;

class ModelDataCollector {
DISALLOW_COPY_AND_ASSIGN_(ModelDataCollector)

    void record_1_migration(Person *pPerson, const int &from, const int &to);

POINTER_PROPERTY(Model, model)

PROPERTY_REF(DoubleVector, total_immune_by_location)

PROPERTY_REF(DoubleVector2, total_immune_by_location_age_class)

PROPERTY_REF(IntVector, popsize_by_location)

PROPERTY_REF(IntVector, popsize_residence_by_location)

PROPERTY_REF(IntVector2, popsize_by_location_age_class)

PROPERTY_REF(IntVector2, popsize_by_location_age_class_by_5)

PROPERTY_REF(IntVector2, popsize_by_location_hoststate)

PROPERTY_REF(DoubleVector, blood_slide_prevalence_by_location)

PROPERTY_REF(DoubleVector2, blood_slide_number_by_location_age_group)

PROPERTY_REF(DoubleVector2, blood_slide_prevalence_by_location_age_group)

PROPERTY_REF(DoubleVector2, blood_slide_number_by_location_age_group_by_5)

PROPERTY_REF(DoubleVector2, blood_slide_prevalence_by_location_age_group_by_5)

PROPERTY_REF(DoubleVector, fraction_of_positive_that_are_clinical_by_location)

PROPERTY_REF(LongVector, total_number_of_bites_by_location)

PROPERTY_REF(LongVector, total_number_of_bites_by_location_year)

PROPERTY_REF(LongVector, person_days_by_location_year)

PROPERTY_REF(DoubleVector2, EIR_by_location_year)

PROPERTY_REF(DoubleVector, EIR_by_location)

PROPERTY_REF(LongVector, cumulative_clinical_episodes_by_location)

PROPERTY_REF(LongVector2, cumulative_clinical_episodes_by_location_age)

PROPERTY_REF(LongVector2, cumulative_clinical_episodes_by_location_age_group)

PROPERTY_REF(DoubleVector2, average_number_biten_by_location_person)

PROPERTY_REF(DoubleVector, percentage_bites_on_top_20_by_location)

PROPERTY_REF(DoubleVector, cumulative_discounted_NTF_by_location)

PROPERTY_REF(DoubleVector, cumulative_NTF_by_location)

PROPERTY_REF(IntVector, today_TF_by_location)

PROPERTY_REF(IntVector, today_number_of_treatments_by_location)

PROPERTY_REF(IntVector, today_RITF_by_location)

PROPERTY_REF(IntVector2, total_number_of_treatments_60_by_location)

PROPERTY_REF(IntVector2, total_RITF_60_by_location)

PROPERTY_REF(IntVector2, total_TF_60_by_location)

PROPERTY_REF(DoubleVector, current_RITF_by_location)

PROPERTY_REF(DoubleVector, current_TF_by_location)

PROPERTY_REF(IntVector, cumulative_mutants_by_location)

PROPERTY_REF(int, current_utl_duration)

PROPERTY_REF(IntVector, UTL_duration)

PROPERTY_REF(IntVector, number_of_treatments_with_therapy_ID)

PROPERTY_REF(IntVector, number_of_treatments_success_with_therapy_ID)

PROPERTY_REF(IntVector, number_of_treatments_fail_with_therapy_ID)

PROPERTY_REF(double, AMU_per_parasite_pop)

PROPERTY_REF(double, AMU_per_person)

PROPERTY_REF(double, AMU_for_clinical_caused_parasite)

PROPERTY_REF(double, AFU)

PROPERTY_REF(double, discounted_AMU_per_parasite_pop)

PROPERTY_REF(double, discounted_AMU_per_person)

PROPERTY_REF(double, discounted_AMU_for_clinical_caused_parasite)

PROPERTY_REF(double, discounted_AFU)


PROPERTY_REF(IntVector2, multiple_of_infection_by_location)

PROPERTY_REF(DoubleVector, current_EIR_by_location)

PROPERTY_REF(IntVector, last_update_total_number_of_bites_by_location)

READ_ONLY_PROPERTY_REF(ResistanceTracker, resistance_tracker)

PROPERTY_REF(DoubleVector2, last_10_blood_slide_prevalence_by_location)

PROPERTY_REF(DoubleVector2, last_10_blood_slide_prevalence_by_location_age_class)

PROPERTY_REF(DoubleVector2, last_10_fraction_positive_that_are_clinical_by_location)

PROPERTY_REF(DoubleVector3, last_10_fraction_positive_that_are_clinical_by_location_age_class)

PROPERTY_REF(DoubleVector3, last_10_fraction_positive_that_are_clinical_by_location_age_class_by_5)

PROPERTY_REF(IntVector, total_parasite_population_by_location)

PROPERTY_REF(IntVector, number_of_positive_by_location)

PROPERTY_REF(IntVector2, total_parasite_population_by_location_age_group)

PROPERTY_REF(IntVector2, number_of_positive_by_location_age_group)

PROPERTY_REF(IntVector2, number_of_clinical_by_location_age_group)

PROPERTY_REF(IntVector2, number_of_clinical_by_location_age_group_by_5)

PROPERTY_REF(IntVector2, number_of_death_by_location_age_group)

PROPERTY_REF(IntVector2, number_of_untreated_cases_by_location_age_year)

PROPERTY_REF(IntVector2, number_of_treatments_by_location_age_year)

PROPERTY_REF(IntVector2, number_of_deaths_by_location_age_year)

PROPERTY_REF(IntVector2, number_of_malaria_deaths_by_location_age_year)

PROPERTY_REF(IntVector3, number_of_treatments_by_location_age_therapy_year)

PROPERTY_REF(IntVector3, number_of_treatment_failures_by_location_age_therapy_year)

PROPERTY_REF(IntVector, monthly_number_of_treatment_by_location);

PROPERTY_REF(IntVector, monthly_number_of_clinical_episode_by_location);


PROPERTY_REF(IntVector2, popsize_by_location_age)


PROPERTY_REF(double, TF_at_15)

PROPERTY_REF(double, single_resistance_frequency_at_15)

PROPERTY_REF(double, double_resistance_frequency_at_15)

PROPERTY_REF(double, triple_resistance_frequency_at_15)

PROPERTY_REF(double, quadruple_resistance_frequency_at_15)

PROPERTY_REF(double, quintuple_resistance_frequency_at_15)

PROPERTY_REF(double, art_resistance_frequency_at_15)

PROPERTY_REF(double, total_resistance_frequency_at_15)

PROPERTY_REF(IntVector, today_TF_by_therapy)

PROPERTY_REF(IntVector, today_number_of_treatments_by_therapy)

PROPERTY_REF(DoubleVector, current_TF_by_therapy)

PROPERTY_REF(IntVector2, total_number_of_treatments_60_by_therapy)

PROPERTY_REF(IntVector2, total_TF_60_by_therapy)

    static const int number_of_reported_MOI = 8;
    mean_acc acc;

public:
    ModelDataCollector(Model *model = NULL);

    //    Statistic(const Statistic& orig);
    virtual ~ModelDataCollector();

    void initialize();

    void perform_population_statistic();

    void perform_yearly_update();

    void perform_monthly_update();


    virtual void collect_number_of_bites(const int &location, const int &number_of_bites);

    virtual void collect_1_clinical_episode(const int &location, const int &age, const int &age_class);

    virtual void update_person_days_by_years(const int &location, const int &days);

    void calculate_EIR();

    void
    record_1_death(const int &location, const int &birthday, const int &number_of_times_bitten, const int &age_group,
                   const int &age);

    void record_1_malaria_death(const int &location, const int &age);

    void calculate_percentage_bites_on_top_20();

    void record_1_TF(const int &location, const bool &by_drug);

    void record_1_treatment(const int &location, const int &age, const int &therapy_id);

    void record_1_non_treated_case(const int &location, const int &age);

    void record_1_mutation(const int &location, IntGenotype *from, IntGenotype *to);


    void begin_time_step();

    void end_of_time_step();

    void update_UTL_vector();

    //    void collect_1_non_resistant_treatment(const int& therapy_id);
    void record_1_treatment_failure_by_therapy(const int &location, const int &age, const int &therapy_id);

    void record_1_treatment_success_by_therapy(const int &therapy_id);

    void update_after_run();

    void record_1_RITF(const int &location);

    void record_AMU_AFU(Person *person, Therapy *therapy, ClonalParasitePopulation *clinical_caused_parasite);


    double get_blood_slide_prevalence(const int &location, const int &age_from, const int &age_to);

private:
    void update_average_number_bitten(const int &location, const int &birthday, const int &number_of_times_bitten);

};

#endif /* MODELDATACOLLECTOR_H */

