#ifndef TURNONMUTATIONEVENT_H
#define TURNONMUTATIONEVENT_H

#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>

class TurnOnMutationEvent : public Event {
DISALLOW_COPY_AND_ASSIGN(TurnOnMutationEvent)

DISALLOW_MOVE(TurnOnMutationEvent)

  double mutation_probability = 0.0;
  int drug_id = -1;

public:
  explicit TurnOnMutationEvent(const int &at_time, const double &mutation_probability, const int& drug_id);

  ~TurnOnMutationEvent() override = default;

  std::string name() override {
    return "TurnOnMutationEvent";
  }

private:
  void execute() override;
};

#endif // TURNONMUTATIONEVENT_H
