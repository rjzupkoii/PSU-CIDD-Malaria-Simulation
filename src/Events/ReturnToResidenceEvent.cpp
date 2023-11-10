/* 
 * ReturnToResidenceEvent.cpp
 *
 * Implement the event to return the individual to their original location.
 */
#include "ReturnToResidenceEvent.h"

#include "Core/Scheduler.h"
#include "Model.h"
#include "Population/Person.h"
#include "Population/Population.h"

OBJECTPOOL_IMPL(ReturnToResidenceEvent)

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
  auto source_location = person->location();
  person->set_location(person->residence_location());
  Model::POPULATION->notify_movement(source_location, person->residence_location());
}
