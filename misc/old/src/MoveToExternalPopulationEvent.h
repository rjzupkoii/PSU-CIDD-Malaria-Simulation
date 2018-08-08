/* 
 * File:   MoveToExternalPopulationEvent.h
 * Author: Merlin
 *
 * Created on August 22, 2013, 10:16 AM
 */

#ifndef MOVETOEXTERNALPOPULATIONEVENT_H
#define	MOVETOEXTERNALPOPULATIONEVENT_H

#include "Events/Event.h"
#include "Core/PropertyMacro.h"
#include "ObjectPool.h"
#include <string>

class Scheduler;
class Person;

class MoveToExternalPopulationEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN(MoveToExternalPopulationEvent)
    OBJECTPOOL(MoveToExternalPopulationEvent)
public:
    MoveToExternalPopulationEvent();
    //    MoveToExternalPopulationEvent(const MoveToExternalPopulationEvent& orig);
    virtual ~MoveToExternalPopulationEvent();
    
    static void schedule_event(Scheduler* scheduler, Person* p, const int& time);

    virtual std::string name() {
        return "MoveToExternalPopulationEvent";
    }
private:
    virtual void execute();

};

#endif	/* MOVETOEXTERNALPOPULATIONEVENT_H */

