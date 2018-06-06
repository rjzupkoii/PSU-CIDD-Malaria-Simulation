/* 
 * File:   ProgressToClinicalEvent.h
 * Author: Merlin
 *
 * Created on July 30, 2013, 2:36 PM
 */

#ifndef PROGRESSTOCLINICALEVENT_H
#define	PROGRESSTOCLINICALEVENT_H

#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"



class Person;
class Scheduler;
class ClonalParasitePopulation;

class ProgressToClinicalEvent : public Event {
    OBJECTPOOL(ProgressToClinicalEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);

public:
    ProgressToClinicalEvent();
    ProgressToClinicalEvent(const ProgressToClinicalEvent& orig);
    virtual ~ProgressToClinicalEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time);

    void receive_no_treatment_routine(Person* p);

    virtual std::string name() {
        return "ProgressToClinicalEvent";
    }
private:
    virtual void execute();
};

#endif	/* PROGRESSTOCLINICALEVENT_H */

