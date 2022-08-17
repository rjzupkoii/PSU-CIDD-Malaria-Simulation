/*
 * RaptEvent.h
 *
 * Define the RAPT event class.
 */

#pragma once

#include "Event.h"

class Person;

class RaptEvent : public Event {
public:
  RaptEvent(const RaptEvent &) = delete;
  RaptEvent &operator=(const RaptEvent &) = delete;

public:
  RaptEvent() = default;
  ~RaptEvent() override = default;

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override { return "RAPT Event"; }

private:
  void execute() override;
};
