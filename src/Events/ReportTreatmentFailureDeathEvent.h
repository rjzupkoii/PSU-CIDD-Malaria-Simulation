/*
 * ReportTreatmentFailureDeathEvent.h
 *
 * Defines the event that reports that an individual died of malaria following treatment.
 */
#ifndef REPORTTREATMENTFAILUREDEATHEVENT_H
#define REPORTTREATMENTFAILUREDEATHEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Event.h"

class Person;
class Scheduler;

class ReportTreatmentFailureDeathEvent : public Event {
  DISALLOW_COPY_AND_ASSIGN(ReportTreatmentFailureDeathEvent)
  OBJECTPOOL(ReportTreatmentFailureDeathEvent)

  PROPERTY_REF(int, age_class)
  PROPERTY_REF(int, location_id)
  PROPERTY_REF(int, therapy_id)

public:
  ReportTreatmentFailureDeathEvent();
  ~ReportTreatmentFailureDeathEvent() override;

  static void schedule_event(Scheduler *scheduler, Person *person, const int &therapy_id, const int &time);

  std::string name() override { return "ReportTreatmentFailureDeathEvent"; }

private:
  void execute() override;
};

#endif
