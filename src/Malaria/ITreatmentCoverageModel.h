#ifndef TREATMENTCOVERAGEMODEL_H
#define TREATMENTCOVERAGEMODEL_H
#include <vector>
#include "Core/PropertyMacro.h"

class ITreatmentCoverageModel {
  DISALLOW_COPY_AND_ASSIGN(ITreatmentCoverageModel)
  DISALLOW_MOVE(ITreatmentCoverageModel)
public:
  double monthly_rate;

  std::vector<double> p_treatment_less_than_5;
  std::vector<double> p_treatment_more_than_5;

  ITreatmentCoverageModel() = default;
  virtual ~ITreatmentCoverageModel() = default;
  virtual double get_probability_to_be_treated(const int& location, const int& age);

  virtual void monthly_update() =0;
};
#endif // TREATMENTCOVERAGEMODEL_H
