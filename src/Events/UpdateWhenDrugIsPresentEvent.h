/* 
 * File:   UpdateByHavingDrugEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 12:16 PM
 */

#ifndef UPDATEWHENDRUGISPRESENTEVENT_H
#define    UPDATEWHENDRUGISPRESENTEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include <string>

class ClonalParasitePopulation;

class Scheduler;

class Person;

class UpdateWhenDrugIsPresentEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(UpdateWhenDrugIsPresentEvent)

 OBJECTPOOL(UpdateWhenDrugIsPresentEvent)

 POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

 public:
  UpdateWhenDrugIsPresentEvent();

  //    UpdateByHavingDrugEvent(const UpdateByHavingDrugEvent& orig);
  virtual ~UpdateWhenDrugIsPresentEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, ClonalParasitePopulation *clinical_caused_parasite,
                             const int &time);

  std::string name() override {
    return "UpdateByHavingDrugEvent";
  }

 private:
  void execute() override;
};

#endif    /* UPDATEWHENDRUGISPRESENTEVENT_H */
