/*
 * RotateStrategyEvent.h
 *
 * Define the event class to rotate treatment strategies on a regular basis.
 */
#ifndef ROTATE_STRATEGY_EVENT_H
#define ROTATE_STRATEGY_EVENT_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class RotateStrategyEvent : public Event {
  PROPERTY_REF(int, new_strategy_id)
  PROPERTY_REF(int, next_strategy_id)
  PROPERTY_REF(int, years)

private:
  void execute() override;

public:
  inline static const std::string EventName = "rotate_treatment_strategy_event";

  RotateStrategyEvent(int at_time, int years, int current_strategy_id, int next_strategy_id);
  ~RotateStrategyEvent() override = default;

  std::string name() override { return EventName; }
};

#endif
