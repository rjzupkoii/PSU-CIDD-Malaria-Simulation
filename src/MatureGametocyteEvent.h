/* 
 * File:   MatureGametocyteEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:38 PM
 */

#ifndef MATUREGAMETOCYTEEVENT_H
#define	MATUREGAMETOCYTEEVENT_H

#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ClonalParasitePopulation;
class Scheduler;
class Person;

class MatureGametocyteEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(MatureGametocyteEvent);
    OBJECTPOOL(MatureGametocyteEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, blood_parasite);

public:
    MatureGametocyteEvent();
    //    MatureGametocyteEvent(const MatureGametocyteEvent& orig);
    virtual ~MatureGametocyteEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* blood_parasite, const int& time);

    virtual std::string name() {
        return "MatureGametocyteEvent";
    }
private:
    virtual void execute();
};

#endif	/* MATUREGAMETOCYTEEVENT_H */

