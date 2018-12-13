#ifndef LINEARTCM_H
#define LINEARTCM_H

#include "ITreatmentCoverageModel.h"

class LinearTCM : public ITreatmentCoverageModel {
 public:

  std::vector<double> p_treatment_less_than_5_to;
  std::vector<double> p_treatment_more_than_5_to;
  int end_time{-1};
  std::vector<double> rate_of_change_under_5;
  std::vector<double> rate_of_change_over_5;

  void monthly_update() override;

  void update_rate_of_change();

};

#endif // LINEARTCM_H
