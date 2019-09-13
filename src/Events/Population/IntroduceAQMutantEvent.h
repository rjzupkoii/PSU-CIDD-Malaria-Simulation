#pragma once

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>

class IntroduceAQMutantEvent : public Event {
DISALLOW_COPY_AND_ASSIGN(IntroduceAQMutantEvent)

DISALLOW_MOVE(IntroduceAQMutantEvent)

OBJECTPOOL(IntroduceAQMutantEvent)

VIRTUAL_PROPERTY_REF(int, location)

VIRTUAL_PROPERTY_REF(double, fraction)

public:
  explicit IntroduceAQMutantEvent(const int& location = -1, const int& execute_at = -1,
                                           const double& fraction = 0);

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceAQMutantEvent() override;

  std::string name() override {
    return "IntroduceAQMutantEvent";
  }

private:
  void execute() override;

};
