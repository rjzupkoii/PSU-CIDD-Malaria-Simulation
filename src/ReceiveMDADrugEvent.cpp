/* 
 * File:   ReceiveMDADrugEvent.cpp
 * Author: Merlin
 * 
 * Created on August 25, 2013, 10:20 PM
 */

#include "ReceiveMDADrugEvent.h"
#include "Person.h"
#include "Scheduler.h"
#include "Therapy.h"

ReceiveMDADrugEvent::ReceiveMDADrugEvent() {
}

ReceiveMDADrugEvent::~ReceiveMDADrugEvent() {
}

void ReceiveMDADrugEvent::schedule_event(Scheduler* scheduler, Person* p, Therapy* therapy, const int& time) {
    if (scheduler != nullptr) {
        ReceiveMDADrugEvent* e = new ReceiveMDADrugEvent();
        e->set_dispatcher(p);
        e->set_received_therapy(therapy);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void ReceiveMDADrugEvent::execute() {
    Person* person = (Person*) dispatcher();
//    if (person->is_in_external_population()) {
//        return;
//    }

    person->receive_therapy(received_therapy_, nullptr);

    person->schedule_update_by_drug_event(nullptr);

}