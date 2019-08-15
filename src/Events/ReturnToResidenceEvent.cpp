/* 
 * File:   ReturnToResidenceEvent.cpp
 * Author: Merlin
 * 
 * Created on August 2, 2013, 11:20 AM
 */

#include <cassert>

#include "ReturnToResidenceEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"

OBJECTPOOL_IMPL(ReturnToResidenceEvent)

ReturnToResidenceEvent::ReturnToResidenceEvent() = default;

ReturnToResidenceEvent::~ReturnToResidenceEvent() = default;

void ReturnToResidenceEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new ReturnToResidenceEvent();
    e->dispatcher = p;
    e->time = time;
    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void ReturnToResidenceEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  person->set_location(person->residence_location());

}
