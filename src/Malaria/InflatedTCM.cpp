#include "InflatedTCM.h"

InflatedTCM::InflatedTCM() = default;

void InflatedTCM::monthly_update() {
  for (auto loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
    p_treatment_less_than_5[loc] *= (1 + monthly_inflation_rate);
    p_treatment_more_than_5[loc] *= (1 + monthly_inflation_rate);

    p_treatment_less_than_5[loc] = p_treatment_less_than_5[loc] > 1.0 ? 1.0 : p_treatment_less_than_5[loc];
    p_treatment_more_than_5[loc] = p_treatment_more_than_5[loc] > 1.0 ? 1.0 : p_treatment_more_than_5[loc];

  }
}
