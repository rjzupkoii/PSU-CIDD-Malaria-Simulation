#define NOMINMAX
#include "SingleRoundMDAEvent.h"
#include "easylogging++.h"
#include "Model.h"
#include "date/date.h"

SingleRoundMDAEvent::SingleRoundMDAEvent(const int& execute_at) {
  time = execute_at;
}

void SingleRoundMDAEvent::execute() {
  LOG(INFO) << date::year_month_day{ scheduler->calendar_date } << ": executing Single Round MDA";
}
