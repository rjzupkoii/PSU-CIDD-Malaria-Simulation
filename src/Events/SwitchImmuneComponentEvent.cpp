/* 
 * File:   SwitchImmuneComponentEvent.cpp
 * Author: Merlin
 * 
 * Created on July 3, 2013, 3:06 PM
 */

#include "SwitchImmuneComponentEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Population/ImmuneSystem.h"
#include "Population/NonInfantImmuneComponent.h"
#include <cassert>

OBJECTPOOL_IMPL(SwitchImmuneComponentEvent)

SwitchImmuneComponentEvent::SwitchImmuneComponentEvent() = default;

SwitchImmuneComponentEvent::~SwitchImmuneComponentEvent() = default;

void SwitchImmuneComponentEvent::execute() {

  assert(dispatcher!=nullptr);
  auto *p = dynamic_cast<Person *>(dispatcher);
  p->immune_system()->set_immune_component(new NonInfantImmuneComponent());

}

void SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Scheduler *scheduler, Person *p,
                                                                            const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new SwitchImmuneComponentEvent();
    e->dispatcher = p;
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}
