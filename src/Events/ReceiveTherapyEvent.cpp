/* 
 * File:   ReceiveTherapyEvent.cpp
 * Author: Merlin
 * 
 * Created on November 4, 2014, 3:00 PM
 */

#include "ReceiveTherapyEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Therapies/Therapy.h"
#include "Population/ClonalParasitePopulation.h"

ReceiveTherapyEvent::ReceiveTherapyEvent() : received_therapy_(nullptr), clinical_caused_parasite_(nullptr) {}

ReceiveTherapyEvent::~ReceiveTherapyEvent() = default;

void ReceiveTherapyEvent::schedule_event(Scheduler *scheduler, Person *p, Therapy *therapy, const int &time,
                                         ClonalParasitePopulation *clinical_caused_parasite) {
  if (scheduler!=nullptr) {
    auto *e = new ReceiveTherapyEvent();
    e->dispatcher = p;
    e->set_received_therapy(therapy);
    e->time = time;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void ReceiveTherapyEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  //    if (person->is_in_external_population()) {
  //        return;
  //    }

  person->receive_therapy(received_therapy_, clinical_caused_parasite_);

  person->schedule_update_by_drug_event(clinical_caused_parasite_);
}
