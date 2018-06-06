/* 
 * File:   EndClinicalByNoTreatmentEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:28 PM
 */

#ifndef ENDCLINICALBYNOTREATMENTEVENT_H
#define	ENDCLINICALBYNOTREATMENTEVENT_H
#include "Event.h"
#include "ObjectPool.h"
#include "PropertyMacro.h"

class ClonalParasitePopulation;
class Scheduler;
class Person;

class EndClinicalByNoTreatmentEvent : public Event {
    DISALLOW_COPY_AND_ASSIGN_(EndClinicalByNoTreatmentEvent);
    OBJECTPOOL(EndClinicalByNoTreatmentEvent)
    POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite);


public:
    EndClinicalByNoTreatmentEvent();
    //    EndClinicalByNoTreatmentEvent(const EndClinicalByNoTreatmentEvent& orig);
    virtual ~EndClinicalByNoTreatmentEvent();
    static void schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time);

    virtual std::string name() {
        return "EndClinicalByNoTreatmentEvent";
    }
private:
    virtual void execute();
};

#endif	/* ENDCLINICALBYNOTREATMENTEVENT_H */

