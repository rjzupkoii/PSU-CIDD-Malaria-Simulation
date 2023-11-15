/* 
 * ClinicalUpdateFunction.cpp
 *
 * Implement the update function for clinical infections.
 */
#include "ClinicalUpdateFunction.h"

#include "Core/Config/Config.h"
#include "Model.h"

ClinicalUpdateFunction::ClinicalUpdateFunction(Model *model) : model_(model) { }

double ClinicalUpdateFunction::get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) {
  return model_->config()->parasite_density_level().log_parasite_density_asymptomatic;
}