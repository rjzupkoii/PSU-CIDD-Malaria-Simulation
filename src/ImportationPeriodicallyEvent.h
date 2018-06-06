/* 
 * File:   ImportationEvent.h
 * Author: Merlin
 *
 * Created on February 21, 2014, 2:42 PM
 */

#ifndef IMPORTATIONPERIODICALLYEVENT_H
#define	IMPORTATIONPERIODICALLYEVENT_H
#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ImportationPeriodicallyEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(ImportationPeriodicallyEvent);
    OBJECTPOOL(ImportationPeriodicallyEvent)
    VIRTUAL_PROPERTY_REF(int, location);
    VIRTUAL_PROPERTY_REF(int, duration);
    VIRTUAL_PROPERTY_REF(int, genotype_id);
    VIRTUAL_PROPERTY_REF(int, number_of_cases);
    VIRTUAL_PROPERTY_REF(int, start_day);
    
public:
    ImportationPeriodicallyEvent(const int& location = -1, const int& duration = -1, const int& genotype_id = -1, const int& number_of_cases = -1, const int& start_day = -1);
    //    ImportationEvent(const ImportationEvent& orig);
    virtual ~ImportationPeriodicallyEvent();

    static void schedule_event(Scheduler* scheduler, const int& location, const int& duration, const int& genotype_id, const int& number_of_cases, const int& start_day);

    virtual std::string name() {
        return "ImportationPeriodicallyEvent";
    }
private:
    virtual void execute();

};

#endif	/* IMPORTATIONPERIODICALLYEVENT_H */

