#include "ChangeStrategyEvent.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"

ChangeStrategyEvent::ChangeStrategyEvent(const int &at_time, const int &strategy_id) : strategy_id(strategy_id) {
  time = at_time;
}

void ChangeStrategyEvent::execute() {
  Model::MODEL->set_treatment_strategy(strategy_id);
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : switch to " << Model::TREATMENT_STRATEGY->name;
}
