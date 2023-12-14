/*
 * RotateStrategyEvent.cpp
 *
 * Implement the class to rotate treatment strategies on a regular basis.
 */
#include "RotateStrategyEvent.h"

#include "Core/Config/Config.h"
#include "Model.h"
#include "Strategies/IStrategy.h"

RotateStrategyEvent::RotateStrategyEvent(int at_time, int years, int new_strategy_id, int next_strategy_id) :
  years_(years), new_strategy_id_(new_strategy_id), next_strategy_id_(next_strategy_id) {
  time = at_time;
}

void RotateStrategyEvent::execute() {
  // Change to the new treatment strategy
  Model::MODEL->set_treatment_strategy(new_strategy_id_);
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : switch to " << Model::TREATMENT_STRATEGY->name();

  // Queue the next iteration of this event
  auto next_time = Model::SCHEDULER->current_time() + (years_ * 365);
  auto* event = new RotateStrategyEvent(next_time, years_, next_strategy_id_, new_strategy_id_);
  scheduler->schedule_population_event(event);
}
