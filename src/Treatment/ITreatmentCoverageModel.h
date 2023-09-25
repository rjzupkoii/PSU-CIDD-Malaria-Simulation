#ifndef TREATMENTCOVERAGEMODEL_H
#define TREATMENTCOVERAGEMODEL_H

#include <vector>
#include "Core/PropertyMacro.h"
#include <yaml-cpp/yaml.h>
#include "Core/Config/CustomConfigItem.h"

class ITreatmentCoverageModel {
 DISALLOW_COPY_AND_ASSIGN(ITreatmentCoverageModel)

 DISALLOW_MOVE(ITreatmentCoverageModel)

 public:
  int starting_time{0};
  std::vector<double> p_treatment_less_than_5;
  std::vector<double> p_treatment_more_than_5;

  ITreatmentCoverageModel() = default;

  virtual ~ITreatmentCoverageModel() = default;

  virtual double get_probability_to_be_treated(const int &location, const int &age);

  virtual void monthly_update() = 0;

  static ITreatmentCoverageModel *build_steady_tcm(const YAML::Node &node, Config *config);

  static void read_p_treatment(const YAML::Node &node, std::vector<double> &p_treatments, std::size_t number_of_locations);

  static ITreatmentCoverageModel *build_inflated_tcm(const YAML::Node &node, Config *config);

  static ITreatmentCoverageModel *build_linear_tcm(const YAML::Node &node, Config *config);

  static ITreatmentCoverageModel *build(const YAML::Node &node, Config *config);
};

#endif // TREATMENTCOVERAGEMODEL_H
