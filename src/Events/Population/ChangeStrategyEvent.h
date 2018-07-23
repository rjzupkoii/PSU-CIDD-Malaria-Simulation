#ifndef CHANGESTRATEGYEVENT_H
#define CHANGESTRATEGYEVENT_H
#include "Events/Event.h"

class ChangeStrategyEvent : public Event {
public:
  int strategy_id{-1};

  ChangeStrategyEvent(const int & at_time, const int& strategy_id);

  virtual ~ChangeStrategyEvent() = default;

  std::string name() override {
    return "ChangeStrategyEvent";
  }

private:
  void execute() override;
};
#endif // CHANGESTRATEGYEVENT_H
