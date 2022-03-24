/* 
 * ReceiveTherapyEvent.h
 *
 * Define the event in which the individual receives a therapy for the infection.
 */
#ifndef RECEIVETHERAPYEVENT_H
#define RECEIVETHERAPYEVENT_H

#include "Event.h"
#include "Population/ClonalParasitePopulation.h"

class Scheduler;

class Person;

class Therapy;

class ClonalParasitePopulation;

class ReceiveTherapyEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(ReceiveTherapyEvent)

 DISALLOW_MOVE(ReceiveTherapyEvent)

 POINTER_PROPERTY(Therapy, received_therapy)

 POINTER_PROPERTY(ClonalParasitePopulation, clinical_caused_parasite)

  PROPERTY_REF(bool, is_mac_therapy)

 public:
  ReceiveTherapyEvent();

  ~ReceiveTherapyEvent() override;

  static void schedule_event(Scheduler *scheduler, Person *p, Therapy *therapy, const int &time,
                             ClonalParasitePopulation *clinical_caused_parasite, bool is_mac_therapy);

  std::string name() override {
    return "ReceiveTherapyEvent";
  }

 private:
  void execute() override;
};

#endif
