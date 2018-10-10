#include "TurnOnMutationEvent.h"
#include "easylogging++.h"
#include "Scheduler.h"
#include "Model.h"
#include "Core/Config/Config.h"


TurnOnMutationEvent::TurnOnMutationEvent(const int& at_time) {
  time = at_time;
}

void TurnOnMutationEvent::execute() {
  for (auto& it : *Model::CONFIG->drug_db()) {
    it.second->p_mutation()=0.005;
  }
  
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : turn mutation on";
}
