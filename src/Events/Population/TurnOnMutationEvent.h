#ifndef TURNONMUTATIONEVENT_H
#define TURNONMUTATIONEVENT_H
#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>

class TurnOnMutationEvent : public Event {
DISALLOW_COPY_AND_ASSIGN(TurnOnMutationEvent)
DISALLOW_MOVE(TurnOnMutationEvent)
public:
  explicit TurnOnMutationEvent(const int& at_time);

  virtual ~TurnOnMutationEvent() = default;

  std::string name() override {
    return "TurnOnMutationEvent";
  }

private:
  void execute() override;
};
#endif // TURNONMUTATIONEVENT_H
