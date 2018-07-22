/* 
 * File:   MoveToExternalPopulationEvent.cpp
 * Author: Merlin
 * 
 * Created on August 22, 2013, 10:16 AM
 */

#include <cmath>
#include "Events/MoveToExternalPopulationEvent.h"
#include "Person.h"
#include "Model.h"
#include "Core/Random.h"
#include "Scheduler.h"
#include "Core/Config/Config.h"
#include "Events/ReturnToNormalPopulationEvent.h"

OBJECTPOOL_IMPL(MoveToExternalPopulationEvent)

MoveToExternalPopulationEvent::MoveToExternalPopulationEvent() {
}

MoveToExternalPopulationEvent::~MoveToExternalPopulationEvent() {
}

void MoveToExternalPopulationEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
  if (scheduler != nullptr) {
    MoveToExternalPopulationEvent *e = new MoveToExternalPopulationEvent();
    e->dispatcher = p;

    e->time = time;


    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void MoveToExternalPopulationEvent::execute() {
  auto person = (Person *) dispatcher();
  person->move_to_external_population();

  //schedule for a return day??
  int length_of_trip = 0;
  while (length_of_trip < 1) {
    length_of_trip = static_cast<int>(ceil(
            Model::RANDOM->random_gamma(Model::CONFIG->circulation_information().length_of_stay_theta,
                                        Model::CONFIG->circulation_information().length_of_stay_k)));
  }

  //            std::cout << length_of_trip << std::endl;
  ReturnToNormalPopulationEvent::schedule_event(Model::SCHEDULER, person,
                                                Model::SCHEDULER->current_time() + length_of_trip);

}