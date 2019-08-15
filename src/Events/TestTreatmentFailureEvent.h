/* 
 * File:   TestTreatmentFailureEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:36 AM
 */

#ifndef TESTTREATMENTFAILUREEVENT_H
#define    TESTTREATMENTFAILUREEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class TestTreatmentFailureEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(TestTreatmentFailureEvent)

 OBJECTPOOL(TestTreatmentFailureEvent)

 POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)
  //    PROPERTY_REF(bool, isResistance);
 PROPERTY_REF(int, therapyId)

 public:
  TestTreatmentFailureEvent();

  //    TestTreatmentFailureEvent(const TestTreatmentFailureEvent& orig);
  virtual ~TestTreatmentFailureEvent();

  static void
  schedule_event(Scheduler *scheduler, Person *p, ClonalParasitePopulation *clinical_caused_parasite, const int &time,
                 const int &t_id = 0);

  std::string name() override {
    return "TestTreatmentFailureEvent";
  }

 private:
  void execute() override;

};

#endif    /* TESTTREATMENTFAILUREEVENT_H */
