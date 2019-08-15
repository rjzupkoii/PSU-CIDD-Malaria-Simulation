/* 
 * File:   EndClinicalEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:27 PM
 */

#include "EndClinicalEvent.h"
#include "Population/Person.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Scheduler.h"
#include "Population/ImmuneSystem.h"
#include "Population/SingleHostClonalParasitePopulations.h"

OBJECTPOOL_IMPL(EndClinicalEvent)

EndClinicalEvent::EndClinicalEvent() : clinical_caused_parasite_(nullptr) {}

EndClinicalEvent::~EndClinicalEvent() = default;

void
EndClinicalEvent::schedule_event(Scheduler *scheduler, Person *p, ClonalParasitePopulation *clinical_caused_parasite,
                                 const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new EndClinicalEvent();
    e->dispatcher = p;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void EndClinicalEvent::execute() {
  auto person = dynamic_cast<Person *>(dispatcher);

  if (person->all_clonal_parasite_populations()->size()==0) {
    person->change_state_when_no_parasite_in_blood();

  } else {
    //still have parasite in blood
    person->immune_system()->set_increase(true);
    person->set_host_state(Person::ASYMPTOMATIC);

    person->determine_relapse_or_not(clinical_caused_parasite_);

  }
}
