#include "IncreasingTCM.h"



void IncreasingTCM::monthly_update() {
  for (auto loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
    p_treatment_less_than_5[loc] += monthly_increase_value_for_less_than_5;
    p_treatment_more_than_5[loc] += monthly_increase_value_for_more_than_5;
  }
}
