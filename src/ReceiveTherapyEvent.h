/* 
 * File:   ReceiveTherapyEvent.h
 * Author: Merlin
 *
 * Created on November 4, 2014, 3:00 PM
 */

#ifndef RECEIVETHERAPYEVENT_H
#define	RECEIVETHERAPYEVENT_H
#include "Event.h"
#include "ClonalParasitePopulation.h"

class Scheduler;
class Person;
class Therapy;
class ClonalParasitePopulation;

class ReceiveTherapyEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(ReceiveTherapyEvent)
    POINTER_PROPERTY(Therapy, received_therapy)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);
public:
    ReceiveTherapyEvent();
    //    ReceiveTherapyEvent(const ReceiveTherapyEvent& orig);
    virtual ~ReceiveTherapyEvent();
    static void schedule_event(Scheduler* scheduler, Person* p, Therapy* therapy, const int& time, ClonalParasitePopulation* clinical_caused_parasite);

    virtual std::string name() {
        return "ReceiveTherapyEvent";
    }
private:
    virtual void execute();
};

#endif	/* RECEIVETHERAPYEVENT_H */

