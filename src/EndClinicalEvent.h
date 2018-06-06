/* 
 * File:   EndClinicalEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:27 PM
 */

#ifndef ENDCLINICALEVENT_H
#define	ENDCLINICALEVENT_H
#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ClonalParasitePopulation;
class Scheduler;
class Person;

class EndClinicalEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(EndClinicalEvent);
    OBJECTPOOL(EndClinicalEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);

public:
    EndClinicalEvent();
    //    EndClinicalEvent(const EndClinicalEvent& orig);
    virtual ~EndClinicalEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time);

    virtual std::string name() {
        return "EndClinicalEvent";
    }
private:
    virtual void execute();
};

#endif	/* ENDCLINICALEVENT_H */

