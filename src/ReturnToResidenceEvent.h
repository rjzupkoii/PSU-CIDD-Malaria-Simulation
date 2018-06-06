/* 
 * File:   ReturnToResidenceEvent.h
 * Author: Merlin
 *
 * Created on August 2, 2013, 11:20 AM
 */

#ifndef RETURNTORESIDENCEEVENT_H
#define	RETURNTORESIDENCEEVENT_H

#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"


class Scheduler;
class Person;

class ReturnToResidenceEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(ReturnToResidenceEvent);
    OBJECTPOOL(ReturnToResidenceEvent)
public:
    ReturnToResidenceEvent();
    //    ReturnToResidenceEvent(const ReturnToResidenceEvent& orig);
    virtual ~ReturnToResidenceEvent();
    static void schedule_event(Scheduler* scheduler, Person* p, const int& time);

    virtual std::string name() {
        return "ReturnToResidenceEvent";
    }
private:
    virtual void execute();
};

#endif	/* RETURNTORESIDENCEEVENT_H */

