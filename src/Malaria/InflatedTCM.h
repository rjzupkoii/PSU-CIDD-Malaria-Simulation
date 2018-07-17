#ifndef INFLATEDICM_H
#define INFLATEDICM_H
#include "ITreatmentCoverageModel.h"

class InlatedTCM : public ITreatmentCoverageModel {
public:
  double monthly_inflation_factor;
  explicit InlatedTCM(const double inflation_factor)
    :monthly_inflation_factor(inflation_factor) {}

  void monthly_update() override;
};
#endif // INFLATEDICM_H
