/* 
 * File:   BirthdayEvent.h
 * Author: nguyentran
 *
 * Created on May 9, 2013, 2:42 PM
 */

#ifndef BIRTHDAYEVENT_H
#define    BIRTHDAYEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include <string>

class Person;

class BirthdayEvent : public Event {
 OBJECTPOOL(BirthdayEvent)

 DISALLOW_COPY_AND_ASSIGN(BirthdayEvent)

 public:
  BirthdayEvent();

  //    BirthdayEvent(const BirthdayEvent& orig);
  virtual ~BirthdayEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override;

 private:
  void execute() override;
};

#endif    /* BIRTHDAYEVENT_H */
