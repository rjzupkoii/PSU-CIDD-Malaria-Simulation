#include "TurnOnMutationEvent.h"
#include "easylogging++.h"
#include "Core/Scheduler.h"
#include "Model.h"
#include "Core/Config/Config.h"

TurnOnMutationEvent::TurnOnMutationEvent(const int &at_time, const double &mutation_probability, const int &drug_id) :
  mutation_probability{mutation_probability}, drug_id{drug_id} {
  time = at_time;
}

void TurnOnMutationEvent::execute() {
  for (auto &it : *Model::CONFIG->drug_db()) {
    if (it.second->id() == drug_id || drug_id == -1) {
      it.second->p_mutation() = mutation_probability;
    }
  }

  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : turn mutation on";
}
