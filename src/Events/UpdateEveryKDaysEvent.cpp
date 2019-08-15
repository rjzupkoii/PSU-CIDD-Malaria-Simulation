/* 
 * File:   UpdateEveryKDaysEvent.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:02 AM
 */

#include "UpdateEveryKDaysEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Model.h"
#include "Core/Config/Config.h"

OBJECTPOOL_IMPL(UpdateEveryKDaysEvent)

UpdateEveryKDaysEvent::UpdateEveryKDaysEvent() = default;

UpdateEveryKDaysEvent::~UpdateEveryKDaysEvent() = default;

void UpdateEveryKDaysEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new UpdateEveryKDaysEvent();
    e->dispatcher = p;
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void UpdateEveryKDaysEvent::execute() {
  auto *person = static_cast<Person *>(dispatcher);
  person->schedule_update_every_K_days_event(Model::CONFIG->update_frequency());
}
