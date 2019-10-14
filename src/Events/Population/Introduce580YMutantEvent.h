//
// Created by Nguyen Tran on 2019-05-01.
//

#ifndef PCMS_INTRODUCE580YPARASITEEVENT_H
#define PCMS_INTRODUCE580YPARASITEEVENT_H


#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>
#include "../../Core/ObjectPool.h"

class Introduce580YMutantEvent : public Event {
  DISALLOW_COPY_AND_ASSIGN(Introduce580YMutantEvent)

    DISALLOW_MOVE(Introduce580YMutantEvent)

    OBJECTPOOL(Introduce580YMutantEvent)

    VIRTUAL_PROPERTY_REF(int, location)

    VIRTUAL_PROPERTY_REF(double, fraction)

public:
  explicit Introduce580YMutantEvent(const int& location = -1, const int& execute_at = -1,
                                    const double& fraction = 0);

  //    ImportationEvent(const ImportationEvent& orig);
  ~Introduce580YMutantEvent() override;

  std::string name() override {
    return "580YImportationEvent";
  }

private:
  void execute() override;

};

#endif //PCMS_INTRODUCE580YPARASITEEVENT_H
