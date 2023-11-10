/* 
 * ReturnToResidenceEvent.h
 *
 * Define the event to return a circulating individual to the original location.
 */

#ifndef RETURNTORESIDENCEEVENT_H
#define RETURNTORESIDENCEEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class Person;
class Scheduler;

class ReturnToResidenceEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(ReturnToResidenceEvent)
 DISALLOW_MOVE(ReturnToResidenceEvent)
 OBJECTPOOL(ReturnToResidenceEvent)

 public:
  ReturnToResidenceEvent() = default;
  virtual ~ReturnToResidenceEvent() = default;

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override {
    return "ReturnToResidenceEvent";
  }

 private:
  void execute() override;
};

#endif
