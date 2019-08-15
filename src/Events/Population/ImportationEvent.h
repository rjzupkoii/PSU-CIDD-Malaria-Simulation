/* 
 * File:   ImportationEvent.h
 * Author: Merlin
 *
 * Created on March 12, 2015, 12:23 PM
 */

#ifndef IMPORTATIONEVENT_H
#define    IMPORTATIONEVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>

class ImportationEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(ImportationEvent)

 DISALLOW_MOVE(ImportationEvent)

 OBJECTPOOL(ImportationEvent)

 VIRTUAL_PROPERTY_REF(int, location)

 VIRTUAL_PROPERTY_REF(int, genotype_id)

 VIRTUAL_PROPERTY_REF(int, number_of_cases)

 public:
  ImportationEvent(const int &location = -1, const int &execute_at = -1, const int &genotype_id = -1,
                   const int &number_of_cases = -1);

  //    ImportationEvent(const ImportationEvent& orig);
  virtual ~ImportationEvent();

  static void schedule_event(Scheduler *scheduler, const int &location, const int &execute_at, const int &genotype_id,
                             const int &number_of_cases);

  std::string name() override {
    return "ImportationEvent";
  }

 private:
  void execute() override;

};

#endif    /* IMPORTATIONEVENT_H */
