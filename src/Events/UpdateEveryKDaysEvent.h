/* 
 * File:   UpdateEveryKDaysEvent.h
 * Author: Merlin
 *
 * Created on August 1, 2013, 12:02 AM
 */

#ifndef UPDATEEVERYKDAYSEVENT_H
#define    UPDATEEVERYKDAYSEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"

class Person;

class UpdateEveryKDaysEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(UpdateEveryKDaysEvent)

 OBJECTPOOL(UpdateEveryKDaysEvent)

 public:
  UpdateEveryKDaysEvent();

  //    UpdateEveryKDaysEvent(const UpdateEveryKDaysEvent& orig);
  virtual ~UpdateEveryKDaysEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, const int &time);

  std::string name() override {
    return "UpdateEveryKDaysEvent";
  }

 private:
  void execute() override;

};

#endif    /* UPDATEEVERYKDAYSEVENT_H */
