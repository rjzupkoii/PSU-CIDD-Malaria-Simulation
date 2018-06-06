/* 
 * File:   ReceiveTherapyEvent.cpp
 * Author: Merlin
 * 
 * Created on November 4, 2014, 3:00 PM
 */

#include "ReceiveTherapyEvent.h"
#include "Person.h"
#include "Scheduler.h"
#include "Therapy.h"
#include "ClonalParasitePopulation.h"

ReceiveTherapyEvent::ReceiveTherapyEvent() {
}

ReceiveTherapyEvent::~ReceiveTherapyEvent() {
}

void ReceiveTherapyEvent::schedule_event(Scheduler* scheduler, Person* p, Therapy* therapy, const int& time, ClonalParasitePopulation* clinical_caused_parasite) {
    if (scheduler != nullptr) {
        ReceiveTherapyEvent* e = new ReceiveTherapyEvent();
        e->set_dispatcher(p);
        e->set_received_therapy(therapy);
        e->set_executable(true);
        e->set_time(time);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        p->add(e);
        scheduler->schedule(e);
    }
}

void ReceiveTherapyEvent::execute() {
    Person* person = (Person*) dispatcher();
    //    if (person->is_in_external_population()) {
    //        return;
    //    }

    person->receive_therapy(received_therapy_,clinical_caused_parasite_);

    person->schedule_update_by_drug_event(clinical_caused_parasite_);
}