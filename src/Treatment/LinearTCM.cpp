#include "LinearTCM.h"
#include "Model.h"

void LinearTCM::monthly_update() {
  if (Model::SCHEDULER->current_time() <= end_time) {
    for (std::size_t loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
      p_treatment_less_than_5[loc] += rate_of_change_under_5[loc];
      p_treatment_more_than_5[loc] += rate_of_change_over_5[loc];
    }
  }
}

void LinearTCM::update_rate_of_change() {

  for (std::size_t loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
    rate_of_change_under_5.push_back(
        30*(p_treatment_less_than_5_to[loc] - p_treatment_less_than_5[loc])/(end_time - starting_time));
    rate_of_change_over_5.push_back(
        30*(p_treatment_more_than_5_to[loc] - p_treatment_more_than_5[loc])/(end_time - starting_time));
  }
}
