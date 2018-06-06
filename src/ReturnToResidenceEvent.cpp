/* 
 * File:   ReturnToResidenceEvent.cpp
 * Author: Merlin
 * 
 * Created on August 2, 2013, 11:20 AM
 */

#include <assert.h>

#include "ReturnToResidenceEvent.h"
#include "Person.h"
#include "Scheduler.h"

OBJECTPOOL_IMPL(ReturnToResidenceEvent)

ReturnToResidenceEvent::ReturnToResidenceEvent() {
}

ReturnToResidenceEvent::~ReturnToResidenceEvent() {
}

void ReturnToResidenceEvent::schedule_event(Scheduler* scheduler, Person* p, const int& time) {
    if (scheduler != nullptr) {
        ReturnToResidenceEvent* e = new ReturnToResidenceEvent();
        e->set_dispatcher(p);

        e->set_executable(true);
        e->set_time(time);

        
        p->add(e);
        scheduler->schedule(e);
    }
}

void ReturnToResidenceEvent::execute() {
    Person* person = (Person*) dispatcher();
    person->set_location(person->residence_location());

}