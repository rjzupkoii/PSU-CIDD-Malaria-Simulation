/* 
 * File:   EndClinicalDueToDrugResistanceEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:24 AM
 */

#ifndef ENDCLINICALDUETODRUGRESISTANCEEVENT_H
#define    ENDCLINICALDUETODRUGRESISTANCEEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class EndClinicalDueToDrugResistanceEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(EndClinicalDueToDrugResistanceEvent)

 OBJECTPOOL(EndClinicalDueToDrugResistanceEvent)

 POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

 public:
  EndClinicalDueToDrugResistanceEvent();

  //    EndClinicalDueToDrugResistanceEvent(const EndClinicalDueToDrugResistanceEvent& orig);
  virtual ~EndClinicalDueToDrugResistanceEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, ClonalParasitePopulation *clinical_caused_parasite,
                             const int &time);

  std::string name() override {
    return "EndClinicalDueToDrugResistanceEvent";
  }

 private:
  void execute() override;

};

#endif    /* ENDCLINICALDUETODRUGRESISTANCEEVENT_H */
