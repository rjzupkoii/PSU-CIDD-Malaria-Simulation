/* 
 * File:   MoveParasiteToBloodEvent.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 11:14 PM
 */

#ifndef MOVEPARASITETOBLOODEVENT_H
#define    MOVEPARASITETOBLOODEVENT_H

#include "Event.h"
#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include <string>

class ClonalParasitePopulation;

class Scheduler;

class Person;

class Genotype;

class MoveParasiteToBloodEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(MoveParasiteToBloodEvent)

 DISALLOW_MOVE(MoveParasiteToBloodEvent)

 OBJECTPOOL(MoveParasiteToBloodEvent)

 POINTER_PROPERTY(Genotype, infection_genotype)

 public:
  MoveParasiteToBloodEvent();

  //    MoveParasiteToBloodEvent(const MoveParasiteToBloodEvent& orig);
  virtual ~MoveParasiteToBloodEvent();

  static void schedule_event(Scheduler *scheduler, Person *p, Genotype *infection_type, const int &time);

  std::string name() override {
    return "MoveParasiteToBloodEvent";
  }

 private:
  void execute() override;
};

#endif    /* MOVEPARASITETOBLOODEVENT_H */
