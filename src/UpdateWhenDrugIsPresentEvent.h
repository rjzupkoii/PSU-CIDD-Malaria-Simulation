/* 
 * File:   UpdateByHavingDrugEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:16 PM
 */

#ifndef UPDATEWHENDRUGISPRESENTEVENT_H
#define	UPDATEWHENDRUGISPRESENTEVENT_H

#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ClonalParasitePopulation;
class Scheduler;
class Person;

class UpdateWhenDrugIsPresentEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(UpdateWhenDrugIsPresentEvent);
    OBJECTPOOL(UpdateWhenDrugIsPresentEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);
public:
    UpdateWhenDrugIsPresentEvent();
    //    UpdateByHavingDrugEvent(const UpdateByHavingDrugEvent& orig);
    virtual ~UpdateWhenDrugIsPresentEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time);

    virtual std::string name() {
        return "UpdateByHavingDrugEvent";
    }
private:
    virtual void execute();
};

#endif	/* UPDATEWHENDRUGISPRESENTEVENT_H */

