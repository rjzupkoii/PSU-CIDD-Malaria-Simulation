/* 
 * File:   CirculateToTargetLocationNextDayEvent.cpp
 * Author: Merlin
 * 
 * Created on August 2, 2013, 10:57 AM
 */

#include "CirculateToTargetLocationNextDayEvent.h"
#include "Person.h"
#include "Model.h"
#include "Random.h"
#include "Config.h"
#include "ReturnToResidenceEvent.h"

OBJECTPOOL_IMPL(CirculateToTargetLocationNextDayEvent)



CirculateToTargetLocationNextDayEvent::CirculateToTargetLocationNextDayEvent() {
}


CirculateToTargetLocationNextDayEvent::~CirculateToTargetLocationNextDayEvent() {
}

void CirculateToTargetLocationNextDayEvent::schedule_event(Scheduler* scheduler, Person* p, const int& target_location, const int& time) {
    if (scheduler != nullptr) {
        CirculateToTargetLocationNextDayEvent* e = new CirculateToTargetLocationNextDayEvent();
        e->set_dispatcher(p);
        e->set_target_location(target_location);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void CirculateToTargetLocationNextDayEvent::execute() {
    Person* person = (Person*) dispatcher();
    person->set_location(target_location_);

    if (target_location_ != person->residence_location()) {
        //if person already have return trip then no need to reschedule it
        //elase
        //Schedule for a return trip in next several days base on gamma distribution
        if (!person->has_return_to_residence_event()) {
            int length_of_trip = 0;
            while (length_of_trip < 1) {
                length_of_trip = Model::RANDOM->random_gamma(Model::CONFIG->circulation_information().length_of_stay_theta, Model::CONFIG->circulation_information().length_of_stay_k);
            }

//            std::cout << length_of_trip << std::endl;
            ReturnToResidenceEvent::schedule_event(Model::SCHEDULER, person, Model::SCHEDULER->current_time() + length_of_trip);

        }
    } else {
        //return by chance so we cancel all return event
        //cancel return trip and do nothing
        person->cancel_all_return_to_residence_events();
    }



}