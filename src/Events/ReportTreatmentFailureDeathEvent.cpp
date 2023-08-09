/*
 * ReportTreatmentFailureDeathEvent.cpp
 *
 * Implement the event to report that an individual died of malaria following treatment.
 */
#include "ReportTreatmentFailureDeathEvent.h"

#include "Core/Scheduler.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/Person.h"

ReportTreatmentFailureDeathEvent::ReportTreatmentFailureDeathEvent() : age_class_(0), location_id_(0), therapy_id_(0) { }

ReportTreatmentFailureDeathEvent::~ReportTreatmentFailureDeathEvent() = default;

void ReportTreatmentFailureDeathEvent::schedule_event(Scheduler *scheduler, Person *person, const int &therapy_id, const int &time) {
  // Ensure that the scheduler exists
  assert(scheduler != nullptr);

  // Create the event to be added to the queue
  auto *event = new ReportTreatmentFailureDeathEvent();
  event->time = time;
  event->set_age_class(person->age_class());
  event->set_location_id(person->location());
  event->set_therapy_id(therapy_id);
  scheduler->schedule_individual_event(event);
}

void ReportTreatmentFailureDeathEvent::execute() {
  Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(location_id(), age_class(), therapy_id());
}
