/* 
 * File:   MoveParasiteToBloodEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:14 PM
 */

#ifndef MOVEPARASITETOBLOODEVENT_H
#define	MOVEPARASITETOBLOODEVENT_H
#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"


class ClonalParasitePopulation;
class Scheduler;
class Person;
class IntGenotype;

class MoveParasiteToBloodEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(MoveParasiteToBloodEvent);
    OBJECTPOOL(MoveParasiteToBloodEvent)
    POINTER_PROPERTY(IntGenotype, infection_type);

public:
    MoveParasiteToBloodEvent();
    //    MoveParasiteToBloodEvent(const MoveParasiteToBloodEvent& orig);
    virtual ~MoveParasiteToBloodEvent();
    static void schedule_event(Scheduler* scheduler, Person* p, IntGenotype* infection_type, const int& time);

    virtual std::string name() {
        return "MoveParasiteToBloodEvent";
    }
private:
    virtual void execute();
};

#endif	/* MOVEPARASITETOBLOODEVENT_H */

