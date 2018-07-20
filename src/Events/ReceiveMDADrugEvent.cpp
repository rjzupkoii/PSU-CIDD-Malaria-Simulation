/* 
 * File:   ReceiveMDADrugEvent.cpp
 * Author: Merlin
 * 
 * Created on August 25, 2013, 10:20 PM
 */

#include "ReceiveMDADrugEvent.h"
#include "Person.h"
#include "Scheduler.h"
#include "Therapies/Therapy.h"

ReceiveMDADrugEvent::ReceiveMDADrugEvent() = default;

ReceiveMDADrugEvent::~ReceiveMDADrugEvent() = default;

void ReceiveMDADrugEvent::schedule_event(Scheduler* scheduler, Person* p, Therapy* therapy, const int& time) {
  if (scheduler != nullptr) {
    auto* e = new ReceiveMDADrugEvent();
    e->dispatcher = p;
    e->set_received_therapy(therapy);
    e->executable = true;
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void ReceiveMDADrugEvent::execute() {
  auto* person = dynamic_cast<Person*>(dispatcher);
  //    if (person->is_in_external_population()) {
  //        return;
  //    }

  person->receive_therapy(received_therapy_, nullptr);

  person->schedule_update_by_drug_event(nullptr);

}
