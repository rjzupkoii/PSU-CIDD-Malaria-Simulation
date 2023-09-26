/*
 * SteadyTCM.hxx
 *
 * Define the steady treatment coverage model, this is the default for the simulation unless otherwise overridden.
 */
#ifndef STEADYICM_H
#define STEADYICM_H

#include "ITreatmentCoverageModel.h"

class SteadyTCM : public ITreatmentCoverageModel {
 public:
  void monthly_update() override { }
};

#endif
