#ifndef INCREASINGTCM_H
#define INCREASINGTCM_H
#include "ITreatmentCoverageModel.h"

class IncreasingTCM : public ITreatmentCoverageModel {
public:
  double monthly_increase_value_for_less_than_5{0};
  double monthly_increase_value_for_more_than_5{0};
  void monthly_update() override;
};
#endif // INCREASINGTCM_H
