/* 
 * File:   Drug.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 1:47 PM
 */

#include "Drug.h"
#include "DrugType.h"
#include "Person.h"
#include "Random.h"
#include "Population.h"
#include "Model.h"
#include "Scheduler.h"
#include "Config.h"
#include <math.h>

OBJECTPOOL_IMPL(Drug)

Drug::Drug(DrugType* drug_type) : drug_type_(drug_type), dosing_days_(0), last_update_time_(0), last_update_value_(1.0), starting_value_(1.0), start_time_(0), end_time_(0), person_drugs_(nullptr) {
}


Drug::~Drug() {

}

void Drug::update() {
    int current_time = Model::SCHEDULER->current_time();
    last_update_value_ = get_current_drug_concentration(current_time);
    last_update_time_ = current_time;
}

double Drug::get_current_drug_concentration(int currentTime) {

    int days = currentTime - start_time_;
    if (days == 0) {
        return 0;
    }

    if (days <= dosing_days_) {
        if (drug_type()->is_artemisinin()) {
            //            std::cout << "hello" << std::endl;
//            double sd = drug_type_->age_group_specific_drug_concentration_sd()[person_drugs_->person()->age_class()];
//            starting_value_ = Model::RANDOM->random_normal_truncated(1.0, sd);
            return starting_value_ + Model::RANDOM->random_uniform_double(-0.1, 0.1);
//            return starting_value_;
        }

        starting_value_ += Model::RANDOM->random_uniform_double(0, 0.1);
        //        return starting_value_ + Model::RANDOM->random_uniform_double(-0.1, 0.1);
        return starting_value_;
    } else {
        double temp = drug_type_->drug_half_life() == 0 ? -100 : -(days - dosing_days_) * log(2) / drug_type_->drug_half_life(); //-ai*t = - t* ln2 / tstar
        if (exp(temp) <= (10.0 / 100.0)) {
            return 0;
        }
        return starting_value_ * exp(temp);
    }
}

double Drug::get_mutation_probability(double currentDrugConcentration) {
    double P = 0;
    if (currentDrugConcentration <= 0)
        return 0;
    if (currentDrugConcentration < (0.5))
        P = 2 * drug_type_->p_mutation() * drug_type_->k() * currentDrugConcentration;

    else if (currentDrugConcentration >= (0.5) && currentDrugConcentration < 1.0) {
        P = drug_type_->p_mutation() * (2 * (1 - drug_type_->k()) * currentDrugConcentration + (2 * drug_type_->k() - 1));
    } else if (currentDrugConcentration >= 1.0) {
        P = drug_type_->p_mutation();
    }
    //    cout << P << endl;
    return P;
}

double Drug::get_mutation_probability() {
    return get_mutation_probability(last_update_value_);
}

void Drug::set_number_of_dosing_days(int dosingDays) {

    dosing_days_ = dosingDays;

    last_update_value_ = 1.0;
    last_update_time_ = Model::SCHEDULER->current_time();

    start_time_ = last_update_time_;
    end_time_ = last_update_time_ + drug_type_->get_total_duration_of_drug_activity(dosingDays);
}

double Drug::get_parasite_killing_rate(int &genotype_id) {
    return drug_type_->get_parasite_killing_rate_by_concentration(last_update_value_,
                                               Model::CONFIG->EC50_power_n_table()[genotype_id][drug_type()->id()]);
}
