/* 
 * File:   ReturnToResidenceEvent.h
 * Author: Merlin
 *
 * Created on August 2, 2013, 11:20 AM
 */

#ifndef RETURNTORESIDENCEEVENT_H
#define    RETURNTORESIDENCEEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include <string>

class Scheduler;

class Person;

class ReturnToResidenceEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(ReturnToResidenceEvent)

 DISALLOW_MOVE(ReturnToResidenceEvent)

 OBJECTPOOL(ReturnToResidenceEvent)

 public:
  ReturnToResidenceEvent();

  //    ReturnToResidenceEvent(const ReturnToResidenceEvent& orig);
  virtual ~ReturnToResidenceEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override {
    return "ReturnToResidenceEvent";
  }

 private:
  void execute() override;
};

#endif    /* RETURNTORESIDENCEEVENT_H */
