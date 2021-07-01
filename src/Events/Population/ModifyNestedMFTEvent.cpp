#include "ModifyNestedMFTEvent.h"
#include "Model.h"
#include "Strategies/IStrategy.h"
#include "Strategies/NestedMFTMultiLocationStrategy.h"
#include "Core/Config/Config.h"
#include "Strategies/NestedMFTStrategy.h"

ModifyNestedMFTEvent::ModifyNestedMFTEvent(const int &at_time, const int &strategy_id) : strategy_id(strategy_id) {
  time = at_time;
}

void ModifyNestedMFTEvent::execute() {
  IStrategy *new_strategy = nullptr;
  if (Model::TREATMENT_STRATEGY->get_type()==IStrategy::NestedMFTMultiLocation) {
    new_strategy = Model::CONFIG->strategy_db()[strategy_id];
    dynamic_cast<NestedMFTMultiLocationStrategy *>(Model::TREATMENT_STRATEGY)->strategy_list[0] = new_strategy;
    new_strategy->adjust_started_time_point(Model::SCHEDULER->current_time());
  }

  if (Model::TREATMENT_STRATEGY->get_type()==IStrategy::NestedMFT) {
    new_strategy = Model::CONFIG->strategy_db()[strategy_id];
    dynamic_cast<NestedMFTStrategy *>(Model::TREATMENT_STRATEGY)->strategy_list[0] = new_strategy;
    new_strategy->adjust_started_time_point(Model::SCHEDULER->current_time());
  }

  if (new_strategy==nullptr) {
    LOG(FATAL) << "Modify Nested MFT Event error with null ptr.";
    exit(EXIT_FAILURE);
  }

  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : switch first strategy in nested strategies to "
            << new_strategy->name;
}
