/* 
 * File:   UpdateEveryKDaysEvent.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:02 AM
 */

#include "UpdateEveryKDaysEvent.h"
#include "Person.h"
#include "Scheduler.h"
#include "Model.h"
#include "Config.h"

OBJECTPOOL_IMPL(UpdateEveryKDaysEvent)

UpdateEveryKDaysEvent::UpdateEveryKDaysEvent() {
}

UpdateEveryKDaysEvent::~UpdateEveryKDaysEvent() {
}

void UpdateEveryKDaysEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
    if (scheduler != NULL) {
        UpdateEveryKDaysEvent *e = new UpdateEveryKDaysEvent();
        e->set_dispatcher(p);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void UpdateEveryKDaysEvent::execute() {
    Person *person = (Person *) dispatcher();
    person->schedule_update_every_K_days_event(Model::CONFIG->update_frequency());
}