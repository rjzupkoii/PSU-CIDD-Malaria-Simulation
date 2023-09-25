

#ifndef STEADYICM_H
#define STEADYICM_H

#include "ITreatmentCoverageModel.h"

class SteadyTCM : public ITreatmentCoverageModel {
 public:
  void monthly_update() override;
};

#endif // STEADYICM_H
