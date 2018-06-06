/* 
 * File:   SwitchImmuneComponentEvent.cpp
 * Author: Merlin
 * 
 * Created on July 3, 2013, 3:06 PM
 */

#include "SwitchImmuneComponentEvent.h"
#include "Person.h"
#include "Scheduler.h"
#include "ImmuneSystem.h"
#include "NonInfantImmuneComponent.h"
#include <assert.h>

OBJECTPOOL_IMPL(SwitchImmuneComponentEvent)

SwitchImmuneComponentEvent::SwitchImmuneComponentEvent() {
}

SwitchImmuneComponentEvent::~SwitchImmuneComponentEvent() {
}

void SwitchImmuneComponentEvent::execute() {

    assert(dispatcher() != nullptr);
    Person* p = (Person*) dispatcher();
    p->immune_system()->set_immune_component(new NonInfantImmuneComponent());


}

void SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Scheduler* scheduler, Person* p, const int& time) {
     if (scheduler != nullptr) {
        SwitchImmuneComponentEvent* e = new SwitchImmuneComponentEvent();
        e->set_dispatcher(p);
        
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}
