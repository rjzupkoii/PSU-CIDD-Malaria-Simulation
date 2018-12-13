#ifndef INFLATEDICM_H
#define INFLATEDICM_H

#include "ITreatmentCoverageModel.h"

class InflatedTCM : public ITreatmentCoverageModel {
 public:
  double monthly_inflation_rate{0};

  InflatedTCM();

  void monthly_update() override;
};

#endif // INFLATEDICM_H
