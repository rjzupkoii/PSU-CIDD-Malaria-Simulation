#include "InflatedTCM.h"

void InlatedTCM::monthly_update() {
  for (auto loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
    p_treatment_less_than_5[loc] *= monthly_inflation_factor;
    p_treatment_more_than_5[loc] *= monthly_inflation_factor;
  }
}
