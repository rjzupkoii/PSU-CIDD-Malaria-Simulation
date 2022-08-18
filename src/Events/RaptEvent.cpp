/*
 * RaptEvent.cpp
 *
 * Implement the RAPT event class. When this event is active we assume that it
 * will run at least once per year, similar to individual birthday events. It
 * may run more often depending upon the period (in months) that the event has
 * been configured for.
 */
#include "RaptEvent.h"

#include <date/date.h>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Core/Scheduler.h"
#include "Events/TestTreatmentFailureEvent.h"
#include "Model.h"
#include "Population/Person.h"

void RaptEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
  if (scheduler != nullptr && Model::CONFIG->rapt_config().isDefined) {
    auto *rapt_event = new RaptEvent();
    rapt_event->dispatcher = p;
    rapt_event->time = time;

    p->add(rapt_event);
    scheduler->schedule_individual_event(rapt_event);
  }
}

void RaptEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  const auto raptConfig = Model::CONFIG->rapt_config();

  // Check to see if we should receive a therapy: RAPT is currently active, the person is the correct age, and they have
  // not recently taken a treatment in the past 28 days (based on testing for treatment failure).
  if (Model::SCHEDULER->current_time() >= raptConfig.start_day
      && person->age() >= raptConfig.age_start
      && !person->has_event<TestTreatmentFailureEvent>()) {

    // Is their base compliance over-5 or under-5 treatment rate?
    auto pr_treatment = person->age() > 5
            ? Model::CONFIG->location_db()[person->location()].p_treatment_more_than_5
            : Model::CONFIG->location_db()[person->location()].p_treatment_less_than_5;

    // Adjust the probability based upon the configured compliance rate with RAPT
    double pr_rapt = pr_treatment * raptConfig.compliance;
    const auto pr = Model::RANDOM->random_flat(0.0, 1.0);

    if (pr <= pr_rapt) {
      person->receive_therapy(Model::CONFIG->therapy_db()[raptConfig.therapy_id], nullptr);
    }
  }

  // Determine when the next RAPT dose should take place based upon scheduling period
  const auto ymd = date::year_month_day(Model::SCHEDULER->calendar_date) + date::months(Model::CONFIG->rapt_config().period);

  // Find the first and last day of the month of the next dose
  const auto first_day = date::year_month_day(ymd.year(), ymd.month(), date::day(1));
  const auto last_day = date::year_month_day_last { ymd.year(), ymd.month() / date::last };

  // Following adjustment scheduler days, conduct a uniform draw across the next dosing month to determine the actual
  // next date when the RAPT dose may be taken
  const auto from = (date::sys_days(first_day) - Model::SCHEDULER->calendar_date).count();
  const auto to = (date::sys_days(last_day) - Model::SCHEDULER->calendar_date).count();
  const auto days_to_next_event = Model::RANDOM->random_uniform_int(from, to + 1);

  // Schedule the event
  schedule_event(scheduler, person, scheduler->current_time() + (int)days_to_next_event);
}
