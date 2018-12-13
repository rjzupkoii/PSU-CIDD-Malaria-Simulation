#ifndef CHANGESTRATEGYEVENT_H
#define CHANGESTRATEGYEVENT_H

#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include <string>

class ChangeStrategyEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(ChangeStrategyEvent)

 DISALLOW_MOVE(ChangeStrategyEvent)

 public:
  int strategy_id{-1};

  ChangeStrategyEvent(const int &at_time, const int &strategy_id);

  virtual ~ChangeStrategyEvent() = default;

  std::string name() override {
    return "ChangeStrategyEvent";
  }

 private:
  void execute() override;
};

#endif // CHANGESTRATEGYEVENT_H
