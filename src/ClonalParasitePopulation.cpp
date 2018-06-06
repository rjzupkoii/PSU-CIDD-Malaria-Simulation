/* 
 * File:   BloodParasite.cpp
 * Author: Merlin
 * 
 * Created on July 11, 2013, 2:21 PM
 */

#include "ClonalParasitePopulation.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Person.h"
#include "Therapy.h"
#include "Model.h"
#include "Scheduler.h"
#include "Config.h"
#include <math.h>

OBJECTPOOL_IMPL(ClonalParasitePopulation)
const double ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY = -1000;

ClonalParasitePopulation::ClonalParasitePopulation(IntGenotype* genotype) : last_update_log10_parasite_density_(LOG_ZERO_PARASITE_DENSITY), gametocyte_level_(0.0), first_date_in_blood_(-1), genotype_(genotype), update_function_(nullptr) {
}

ClonalParasitePopulation::~ClonalParasitePopulation() {
}

double ClonalParasitePopulation::get_current_parasite_density(const int& current_time) {
    int duration = current_time - parasite_population()->latest_update_time();
    if (duration == 0) {
        return last_update_log10_parasite_density_;
    }

    if (update_function_ == nullptr) {
        //        std::cout << "hello" << std::endl;
        return last_update_log10_parasite_density_;
    }

    return update_function_->get_current_parasite_density(this, duration);
}

void ClonalParasitePopulation::mutate_to(IntGenotype* genotype) {
    //TODO:: do other statistic things
    set_genotype(genotype);
}

double ClonalParasitePopulation::get_log10_relative_density() {

    if ((last_update_log10_parasite_density_ == LOG_ZERO_PARASITE_DENSITY) || (gametocyte_level_ == 0.0))
        return LOG_ZERO_PARASITE_DENSITY;

    return last_update_log10_parasite_density_ + log10(gametocyte_level_);
}

double ClonalParasitePopulation::last_update_log10_parasite_density() const {
    return last_update_log10_parasite_density_;
}

void ClonalParasitePopulation::set_last_update_log10_parasite_density(const double& value) {
    if (last_update_log10_parasite_density_ != value) {
        parasite_population_->remove_all_infection_force();
        last_update_log10_parasite_density_ = value;
        parasite_population_->add_all_infection_force();
    }
}

double ClonalParasitePopulation::gametocyte_level() const {
    return gametocyte_level_;
}

void ClonalParasitePopulation::set_gametocyte_level(const double& value) {
    if (gametocyte_level_ != value) {
        parasite_population_->remove_all_infection_force();
        gametocyte_level_ = value;
        parasite_population_->add_all_infection_force();
    }
}

IntGenotype* ClonalParasitePopulation::genotype() const {

    return genotype_;
}

void ClonalParasitePopulation::set_genotype(IntGenotype* value) {
    if (genotype_ != value) {
        parasite_population_->remove_all_infection_force();
        genotype_ = value;
        parasite_population_->add_all_infection_force();
    }
}

bool ClonalParasitePopulation::resist_to(Therapy* therapy) {
    return genotype_->resist_to(therapy);
}

bool ClonalParasitePopulation::resist_to(DrugType* dt) {
    return genotype_->resist_to(dt);
}

bool ClonalParasitePopulation::resist_to(const int &drug_id) {
    return genotype_->resist_to(Model::CONFIG->drug_db()->drug_db()[drug_id]);
}

void ClonalParasitePopulation::update() {
    set_last_update_log10_parasite_density(get_current_parasite_density(Model::SCHEDULER->current_time()));
}

void ClonalParasitePopulation::perform_drug_action(const double& percent_parasite_remove) {
    double newSize = last_update_log10_parasite_density_;
    if (percent_parasite_remove > 1) {
        newSize = Model::CONFIG->log_parasite_density_level().log_parasite_density_cured;
    } else {
        newSize += log10(1 - percent_parasite_remove);
    }


    if (newSize < Model::CONFIG->log_parasite_density_level().log_parasite_density_cured) {
        newSize = Model::CONFIG->log_parasite_density_level().log_parasite_density_cured;
    }
    set_last_update_log10_parasite_density(newSize);
}