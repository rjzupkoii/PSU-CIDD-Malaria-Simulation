/* 
 * File:   BirthdayEvent.cpp
 * Author: nguyentran
 * 
 * Created on May 9, 2013, 2:42 PM
 */

#include <cassert>

#include "BirthdayEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Helpers/TimeHelpers.h"
#include "easylogging++.h"

OBJECTPOOL_IMPL(BirthdayEvent)

BirthdayEvent::BirthdayEvent() = default;

BirthdayEvent::~BirthdayEvent() = default;

void BirthdayEvent::execute() {
  assert(dispatcher!=nullptr);
  auto *person = dynamic_cast<Person *>(dispatcher);
  person->increase_age_by_1_year();

  const auto days_to_next_year = TimeHelpers::number_of_days_to_next_year(scheduler->calendar_date);

  schedule_event(scheduler, person, scheduler->current_time() + days_to_next_year);
}

void BirthdayEvent::schedule_event(Scheduler *scheduler, Person *p, const int &time) {
  if (scheduler!=nullptr) {
    auto *birthday_event = new BirthdayEvent();
    birthday_event->dispatcher = p;
    birthday_event->time = time;

    p->add(birthday_event);
    scheduler->schedule_individual_event(birthday_event);
    //        std::cout << scheduler->current_time() << " - hello" << std::endl;
  }
}

std::string BirthdayEvent::name() {
  return "Birthday Event";
}
