/* 
 * File:   MatureGametocyteEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:38 PM
 */

#include "MatureGametocyteEvent.h"
#include "Person.h"
#include "ClonalParasitePopulation.h"
#include "Scheduler.h"
#include "SingleHostClonalParasitePopulations.h"
#include "DrugsInBlood.h"
#include "Model.h"
#include "Config.h"

OBJECTPOOL_IMPL(MatureGametocyteEvent)

MatureGametocyteEvent::MatureGametocyteEvent() {
}

MatureGametocyteEvent::~MatureGametocyteEvent() {
}

void MatureGametocyteEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* blood_parasite, const int& time) {
    if (scheduler != nullptr) {
        MatureGametocyteEvent* e = new MatureGametocyteEvent();
        e->set_dispatcher(p);
        e->set_blood_parasite(blood_parasite);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void MatureGametocyteEvent::execute() {
    Person* person = (Person*) dispatcher();
    if (person->all_clonal_parasite_populations()->contain(blood_parasite_)) {
        if (person->drugs_in_blood()->is_drug_in_blood(1) && !blood_parasite_->resist_to(Model::CONFIG->drug_db()->drug_db()[1])) {
            blood_parasite_->set_gametocyte_level(Model::CONFIG->gametocyte_level_under_artemisinin_action());
        } else {
            blood_parasite_->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
        }
    }


}