/*
 * CirculateToTargetLocationNextDayEvent.h
 *
 * Define the event to move the individual to the next location.
 */
#ifndef CIRCULATETOTARGETLOCATIONNEXTDAYEVENT_H
#define CIRCULATETOTARGETLOCATIONNEXTDAYEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class Person;
class Scheduler;

class CirculateToTargetLocationNextDayEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(CirculateToTargetLocationNextDayEvent)
 DISALLOW_MOVE(CirculateToTargetLocationNextDayEvent)
 OBJECTPOOL(CirculateToTargetLocationNextDayEvent)
 PROPERTY_REF(int, target_location)

 public:
  CirculateToTargetLocationNextDayEvent() : target_location_(0) { }
  ~CirculateToTargetLocationNextDayEvent() override = default;

  static void schedule_event(Scheduler *scheduler, Person *p, const int &target_location, const int &time);
  std::string name() override;

 private:
  void execute() override;
};

#endif
