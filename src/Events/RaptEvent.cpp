#include "RaptEvent.h"

#include <date/date.h>
#include <fmt/core.h>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Core/Scheduler.h"
#include "Events/BirthdayEvent.h"
#include "Events/TestTreatmentFailureEvent.h"
#include "Model.h"
#include "Population/Person.h"

RaptEvent::RaptEvent() = default;

RaptEvent::~RaptEvent() = default;

std::string RaptEvent::name() {
  return "Rapt Event";
}

void RaptEvent::schedule_event(Scheduler *scheduler, Person *p,
                               const int &time) {
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
  // check if executable
  // executable condition:
  // current date > start_date
  // age > start_date
  // not having any test treatment failure event (or having drug in the past 28 days)

  if (Model::SCHEDULER->current_time() >= raptConfig.start_day
      && person->age() >= raptConfig.age_start
      && !person->has_event<TestTreatmentFailureEvent>()) {
    // then execute
    auto pTreatment = person->age() > 5
                          ? Model::CONFIG->location_db()[person->location()]
                                .p_treatment_more_than_5
                          : Model::CONFIG->location_db()[person->location()]
                                .p_treatment_less_than_5;
    double pRapt = pTreatment * raptConfig.compliance;
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);

    if (p <= pRapt) {
      person->receive_therapy(
          Model::CONFIG->therapy_db()[raptConfig.therapy_id], nullptr);
    }
  }

  const auto ymd = date::year_month_day(Model::SCHEDULER->calendar_date)
                   + date::months(Model::CONFIG->rapt_config().period);

  const auto first_day =
      date::year_month_day(ymd.year(), ymd.month(), date::day(1));

  const auto last_day =
      date::year_month_day_last { ymd.year(), ymd.month() / date::last };

  const auto from =
      (date::sys_days(first_day) - Model::SCHEDULER->calendar_date).count();
  const auto to =
      (date::sys_days(last_day) - Model::SCHEDULER->calendar_date).count();

  const auto days_to_next_event =
      Model::RANDOM->random_uniform_int(from, to + 1);

  schedule_event(scheduler, person,
                 scheduler->current_time() + days_to_next_event);
}
