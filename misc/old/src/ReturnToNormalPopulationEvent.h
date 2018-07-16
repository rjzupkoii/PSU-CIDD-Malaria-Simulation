/* 
 * File:   ReturnToNormalPopulationEvent.h
 * Author: Merlin
 *
 * Created on August 22, 2013, 10:22 AM
 */

#ifndef RETURNTONORMALPOPULATIONEVENT_H
#define	RETURNTONORMALPOPULATIONEVENT_H
#include "Core/PropertyMacro.h"
#include "ObjectPool.h"
#include "Events/Event.h"

class Person;
class Scheduler;

class ReturnToNormalPopulationEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN(ReturnToNormalPopulationEvent);
    OBJECTPOOL(ReturnToNormalPopulationEvent)
public:
    ReturnToNormalPopulationEvent();
    //    ReturnToNormalPopulationEvent(const ReturnToNormalPopulationEvent& orig);
    virtual ~ReturnToNormalPopulationEvent();
    
    static void schedule_event(Scheduler* scheduler, Person* p,  const int& time);

    virtual std::string name() {
        return "ReturnToNormalPopulationEvent";
    }
private:
    virtual void execute();
};

#endif	/* RETURNTONORMALPOPULATIONEVENT_H */

