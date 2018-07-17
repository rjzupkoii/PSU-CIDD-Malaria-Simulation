#include "ITreatmentCoverageModel.h"
#include "easylogging++.h"

double ITreatmentCoverageModel::get_probability_to_be_treated(const int& location, const int& age) {
  LOG_IF( location<0||location>=p_treatment_less_than_5.size()|| location >= p_treatment_more_than_5.size(),FATAL)
  << "wrong locaction value: " << location;
  return age <= 5 ? p_treatment_less_than_5[location] : p_treatment_more_than_5[location];
}
