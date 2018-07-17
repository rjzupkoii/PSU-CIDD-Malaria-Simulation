#include "InflatedTCM.h"

void InlatedTCM::monthly_update() {
  // if (current_time_ > Model::CONFIG->start_intervention_day() && is_today_last_day_of_year()) {
  for (auto loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
    p_treatment_less_than_5[loc] *= monthly_inflation_factor;
    p_treatment_more_than_5[loc] *= monthly_inflation_factor;
  }
  // }
}
