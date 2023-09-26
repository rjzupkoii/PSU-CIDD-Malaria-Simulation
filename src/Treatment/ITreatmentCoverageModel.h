/*
 * ITreatmentCoverageModel.h
 *
 * Define the abstract class for the treatment coverage models (TCMs) and the factory pattern for building the various
 * TCMs that can be used by the simulation.
 */
#ifndef TREATMENTCOVERAGEMODEL_H
#define TREATMENTCOVERAGEMODEL_H

#include <vector>
#include <yaml-cpp/yaml.h>

#include "Core/PropertyMacro.h"
#include "Core/Config/CustomConfigItem.h"

class ITreatmentCoverageModel {
  DISALLOW_COPY_AND_ASSIGN(ITreatmentCoverageModel)
  DISALLOW_MOVE(ITreatmentCoverageModel)

  PROPERTY(int, starting_time)

private:
  static ITreatmentCoverageModel *build_steady_tcm(const YAML::Node &node, Config *config);
  static ITreatmentCoverageModel *build_inflated_tcm(const YAML::Node &node, Config *config);
  static ITreatmentCoverageModel *build_linear_tcm(const YAML::Node &node, Config *config);
  static void read_p_treatment(const YAML::Node &node, std::vector<double> &p_treatments, std::size_t number_of_locations);

 public:
  std::vector<double> p_treatment_less_than_5;    // TODO Make this a property
  std::vector<double> p_treatment_more_than_5;    // TODO Make this a property

  ITreatmentCoverageModel() : starting_time_{0} { }

  virtual ~ITreatmentCoverageModel() = default;

  // Get the probability to be treated based upon the individual's location and age, for the default implementation we
  // are presuming under-5 (< 5) or over-5.
  virtual double get_probability_to_be_treated(const int &location, const int &age);

  // Monthly update function for the treatment coverage model.
  virtual void monthly_update() = 0;

  // Factory to build the various treatment coverage models.
  static ITreatmentCoverageModel *build(const YAML::Node &node, Config *config);
};

#endif
