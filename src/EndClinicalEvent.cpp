/* 
 * File:   EndClinicalEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:27 PM
 */

#include "EndClinicalEvent.h"
#include "Person.h"
#include "ClonalParasitePopulation.h"
#include "Scheduler.h"
#include "ImmuneSystem.h"
#include "SingleHostClonalParasitePopulations.h"

OBJECTPOOL_IMPL(EndClinicalEvent)

EndClinicalEvent::EndClinicalEvent() {
}

EndClinicalEvent::~EndClinicalEvent() {
}

void EndClinicalEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time) {
    if (scheduler != NULL) {
        EndClinicalEvent* e = new EndClinicalEvent();
        e->set_dispatcher(p);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void EndClinicalEvent::execute() {
    Person* person = (Person*) dispatcher();

    if (person->all_clonal_parasite_populations()->size() == 0) {
        person->change_state_when_no_parasite_in_blood();

    } else {
        //still have parasite in blood
        person->immune_system()->set_increase(true);
        person->set_host_state(Person::ASYMPTOMATIC);

        person->determine_relapse_or_not(clinical_caused_parasite_);

    }
}