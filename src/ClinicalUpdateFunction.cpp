/* 
 * File:   ClinicalUpdateFunction.cpp
 * Author: Merlin
 * 
 * Created on July 29, 2013, 5:43 PM
 */

#include "ClinicalUpdateFunction.h"
#include "Model.h"
#include "Config.h"

ClinicalUpdateFunction::ClinicalUpdateFunction(Model* model) : model_(model) {
}


ClinicalUpdateFunction::~ClinicalUpdateFunction() {
}

double ClinicalUpdateFunction::get_current_parasite_density(ClonalParasitePopulation* parasite, int duration) {
    
    return model_->config()->log_parasite_density_level().log_parasite_density_asymptomatic;
}