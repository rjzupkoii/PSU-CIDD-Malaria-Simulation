/* 
 * File:   MoveParasiteToBloodEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:14 PM
 */

#include "MoveParasiteToBloodEvent.h"
#include "Person.h"
#include "ImmuneSystem.h"
#include "Model.h"
#include "Random.h"
#include "Config.h"
#include "ClonalParasitePopulation.h"
#include "DrugsInBlood.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Scheduler.h"

OBJECTPOOL_IMPL(MoveParasiteToBloodEvent)

MoveParasiteToBloodEvent::MoveParasiteToBloodEvent() {
}

MoveParasiteToBloodEvent::~MoveParasiteToBloodEvent() {
}

void MoveParasiteToBloodEvent::schedule_event(Scheduler* scheduler, Person* p, IntGenotype* infection_type, const int& time) {
    if (scheduler != nullptr) {
        MoveParasiteToBloodEvent* e = new MoveParasiteToBloodEvent();
        e->set_dispatcher(p);
        e->set_infection_type(infection_type);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void MoveParasiteToBloodEvent::execute() {
    Person* person = (Person*) dispatcher();
    IntGenotype* parasite_type = person->liver_parasite_type();
    person->set_liver_parasite_type(nullptr);

    //add to blood
    if (person->host_state() == Person::EXPOSED) {
        person->set_host_state(Person::ASYMPTOMATIC);
    }

    person->immune_system()->set_increase(true);

    ClonalParasitePopulation* new_parasite = person->add_new_parasite_to_blood(parasite_type);

    new_parasite->set_last_update_log10_parasite_density(Model::RANDOM->random_normal_truncated(Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic, 0.5));

    if (person->drugs_in_blood()->size() > 0) {
        //person has drug in blood
        new_parasite->set_update_function(Model::MODEL->having_drug_update_function());

    } else {

        if (person->all_clonal_parasite_populations()->size() > 1) {
            if (Model::CONFIG->allow_new_coinfection_to_cause_symtoms()) {
                person->determine_clinical_or_not(new_parasite);
            } else {
                new_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());
            }
        } else {
            person->determine_clinical_or_not(new_parasite);
        }

    }

    person->schedule_mature_gametocyte_event(new_parasite);
}