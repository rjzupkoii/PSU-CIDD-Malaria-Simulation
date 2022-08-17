#pragma once
#include "Event.h"

class Person;

class RaptEvent : public Event {
public:
  RaptEvent(const RaptEvent &) = delete;
  RaptEvent &operator=(const RaptEvent &) = delete;

public:
  RaptEvent();

  //    RaptEvent(const BirthdayEvent& orig);
  ~RaptEvent() override;

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override;

private:
  void execute() override;
};
