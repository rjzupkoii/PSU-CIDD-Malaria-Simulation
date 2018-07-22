/* 
 * File:   ReturnToNormalPopulationEvent.cpp
 * Author: Merlin
 * 
 * Created on August 22, 2013, 10:22 AM
 */

#include "Events/ReturnToNormalPopulationEvent.h"
#include "Person.h"
#include "Scheduler.h"

OBJECTPOOL_IMPL(ReturnToNormalPopulationEvent)

ReturnToNormalPopulationEvent::ReturnToNormalPopulationEvent() {
}

ReturnToNormalPopulationEvent::~ReturnToNormalPopulationEvent() {
}

void ReturnToNormalPopulationEvent::schedule_event(Scheduler* scheduler, Person* p, const int& time) {
    if (scheduler != nullptr) {
        ReturnToNormalPopulationEvent* e = new ReturnToNormalPopulationEvent();
        e->dispatcher = p;

        e->time = time;

        p->add(e);
        scheduler->schedule_individual_event(e);
    }
}

void ReturnToNormalPopulationEvent::execute() {
    Person* person = (Person*) dispatcher();
    person->return_to_normal_population();

}