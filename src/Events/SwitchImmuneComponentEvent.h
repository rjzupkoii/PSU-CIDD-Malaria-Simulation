/* 
 * File:   SwitchImmuneComponentEvent.h
 * Author: Merlin
 *
 * Created on July 3, 2013, 3:06 PM
 */

#ifndef SWITCHIMMUNECOMPONENTEVENT_H
#define    SWITCHIMMUNECOMPONENTEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"

class Scheduler;

class Person;

class SwitchImmuneComponentEvent : public Event {
 OBJECTPOOL(SwitchImmuneComponentEvent)

 public:
  SwitchImmuneComponentEvent();

  SwitchImmuneComponentEvent(const SwitchImmuneComponentEvent &orig);

  virtual ~SwitchImmuneComponentEvent();

  static void schedule_for_switch_immune_component_event(Scheduler *scheduler, Person *p, const int &time);

  virtual std::string name() {
    return "SwitchImmuneComponentEvent";
  }

 private:
  virtual void execute();
};

#endif    /* SWITCHIMMUNECOMPONENTEVENT_H */

