/* 
 * File:   TestTreatmentFailureEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:36 AM
 */

#ifndef TESTTREATMENTFAILUREEVENT_H
#define	TESTTREATMENTFAILUREEVENT_H
#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ClonalParasitePopulation;
class Scheduler;
class Person;

class TestTreatmentFailureEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(TestTreatmentFailureEvent);
    OBJECTPOOL(TestTreatmentFailureEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);
//    PROPERTY_REF(bool, isResistance);
    PROPERTY_REF(int, therapyId);

public:
    TestTreatmentFailureEvent();
    //    TestTreatmentFailureEvent(const TestTreatmentFailureEvent& orig);
    virtual ~TestTreatmentFailureEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time, const int& t_id = 0);

    virtual std::string name() {
        return "TestTreatmentFailureEvent";
    }
private:
    virtual void execute();

};

#endif	/* TESTTREATMENTFAILUREEVENT_H */

