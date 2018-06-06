/* 
 * File:   BirthdayEvent.cpp
 * Author: nguyentran
 * 
 * Created on May 9, 2013, 2:42 PM
 */

#include <assert.h>

#include "BirthdayEvent.h"
#include "Person.h"
#include "Scheduler.h"

OBJECTPOOL_IMPL(BirthdayEvent)

BirthdayEvent::BirthdayEvent() : Event() {
}

BirthdayEvent::~BirthdayEvent() {
}

void BirthdayEvent::execute() {
    assert(dispatcher() != nullptr);
    Person* p = (Person*) dispatcher();
    p->increase_age_by_1_year();

    schedule_event(scheduler(), p, scheduler()->current_time() + 360);
}

void BirthdayEvent::schedule_event(Scheduler* scheduler, Person* p, const int& time) {
    if (scheduler != nullptr) {
        BirthdayEvent* e = new BirthdayEvent();
        e->set_dispatcher(p);        
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
//        std::cout << scheduler->current_time() << " - hello" << std::endl;
    }
} 