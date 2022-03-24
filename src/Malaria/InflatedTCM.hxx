#ifndef INFLATEDICM_H
#define INFLATEDICM_H

#include "ITreatmentCoverageModel.h"

class InflatedTCM : public ITreatmentCoverageModel {
 public:
  double monthly_inflation_rate{0};

  InflatedTCM() = default;

  void monthly_update() override {
    for (std::size_t loc = 0; loc < p_treatment_less_than_5.size(); loc++) {
      p_treatment_less_than_5[loc] *= (1 + monthly_inflation_rate);
      p_treatment_more_than_5[loc] *= (1 + monthly_inflation_rate);

      p_treatment_less_than_5[loc] = p_treatment_less_than_5[loc] > 1.0 ? 1.0 : p_treatment_less_than_5[loc];
      p_treatment_more_than_5[loc] = p_treatment_more_than_5[loc] > 1.0 ? 1.0 : p_treatment_more_than_5[loc];

    }
  }
};

#endif // INFLATEDICM_H
