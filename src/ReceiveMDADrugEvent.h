/* 
 * File:   ReceiveMDADrugEvent.h
 * Author: Merlin
 *
 * Created on August 25, 2013, 10:20 PM
 */

#ifndef RECEIVEMDADRUGEVENT_H
#define	RECEIVEMDADRUGEVENT_H
#include "Event.h"

class Scheduler;
class Person;
class Therapy;

class ReceiveMDADrugEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(ReceiveMDADrugEvent)
    POINTER_PROPERTY(Therapy, received_therapy)

public:
    ReceiveMDADrugEvent();
    //    ReceiveMDADrugEvent(const ReceiveMDADrugEvent& orig);
    virtual ~ReceiveMDADrugEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, Therapy* therapy, const int& time);

    virtual std::string name() {
        return "ReceiveMDADrugEvent";
    }
private:
    virtual void execute();

};

#endif	/* RECEIVEMDADRUGEVENT_H */

