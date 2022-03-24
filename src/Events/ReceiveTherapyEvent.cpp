/* 
 * ReceiveTherapyEvent.cpp
 *
 * Implement the event, note the housekeeping for the MAC therapy where the baseline need to be tracked between dosing regimes.
 */
#include "ReceiveTherapyEvent.h"

#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Therapies/Therapy.hxx"
#include "Population/ClonalParasitePopulation.h"

ReceiveTherapyEvent::ReceiveTherapyEvent() : received_therapy_(nullptr), clinical_caused_parasite_(nullptr), is_mac_therapy_(false) {}

ReceiveTherapyEvent::~ReceiveTherapyEvent() = default;

void ReceiveTherapyEvent::schedule_event(Scheduler *scheduler, Person *p, Therapy *therapy, const int &time,
                                         ClonalParasitePopulation *clinical_caused_parasite, bool is_mac_therapy = false) {
  if (scheduler!=nullptr) {
    // Set up the event
    auto *e = new ReceiveTherapyEvent();
    e->dispatcher = p;
    e->set_received_therapy(therapy);
    e->time = time;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->set_is_mac_therapy(is_mac_therapy);

    // Schedule it for the individual
    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void ReceiveTherapyEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  person->receive_therapy(received_therapy_, clinical_caused_parasite_, is_mac_therapy_);
  person->schedule_update_by_drug_event(clinical_caused_parasite_);
}
