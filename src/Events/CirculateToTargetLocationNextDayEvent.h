/* 
 * File:   CirculateToTargetLocationNextDayEvent.h
 * Author: Merlin
 *
 * Created on August 2, 2013, 10:57 AM
 */

#ifndef CIRCULATETOTARGETLOCATIONNEXTDAYEVENT_H
#define    CIRCULATETOTARGETLOCATIONNEXTDAYEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"

class Scheduler;

class Person;

class CirculateToTargetLocationNextDayEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(CirculateToTargetLocationNextDayEvent)

 DISALLOW_MOVE(CirculateToTargetLocationNextDayEvent)

 OBJECTPOOL(CirculateToTargetLocationNextDayEvent)

 PROPERTY_REF(int, target_location)

 public:
  CirculateToTargetLocationNextDayEvent();

  //    CirculateToTargetLocationNextDayEvent(const CirculateToTargetLocationNextDayEvent& orig);
  virtual ~CirculateToTargetLocationNextDayEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, const int &target_location, const int &time);

  std::string name() override;

 private:
  void execute() override;

 private:

};

#endif    /* CIRCULATETOTARGETLOCATIONNEXTDAYEVENT_H */
