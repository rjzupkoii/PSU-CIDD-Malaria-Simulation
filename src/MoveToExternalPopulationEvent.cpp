/* 
 * File:   MoveToExternalPopulationEvent.cpp
 * Author: Merlin
 * 
 * Created on August 22, 2013, 10:16 AM
 */

#include "MoveToExternalPopulationEvent.h"
#include "Person.h"
#include "Model.h"
#include "Random.h"
#include "Scheduler.h"
#include "Config.h"
#include "ReturnToNormalPopulationEvent.h"

OBJECTPOOL_IMPL(MoveToExternalPopulationEvent)

MoveToExternalPopulationEvent::MoveToExternalPopulationEvent() {
}

MoveToExternalPopulationEvent::~MoveToExternalPopulationEvent() {
}

void MoveToExternalPopulationEvent::schedule_event(Scheduler* scheduler, Person* p, const int& time) {
    if (scheduler != nullptr) {
        MoveToExternalPopulationEvent* e = new MoveToExternalPopulationEvent();
        e->set_dispatcher(p);

        e->set_executable(true);
        e->set_time(time);


        p->add(e);
        scheduler->schedule(e);
    }
}

void MoveToExternalPopulationEvent::execute() {
    Person* person = (Person*) dispatcher();
    person->move_to_external_population();

    //schedule for a return day??
    int length_of_trip = 0;
    while (length_of_trip < 1) {
        length_of_trip = ceil(Model::RANDOM->random_gamma(Model::CONFIG->circulation_information().length_of_stay_theta, Model::CONFIG->circulation_information().length_of_stay_k));
    }

    //            std::cout << length_of_trip << std::endl;
    ReturnToNormalPopulationEvent::schedule_event(Model::SCHEDULER, person, Model::SCHEDULER->current_time() + length_of_trip);

}