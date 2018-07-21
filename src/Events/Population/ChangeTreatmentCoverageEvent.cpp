#include "ChangeTreatmentCoverageEvent.h"
#include "Model.h"
#include "Helpers/ObjectHelpers.h"
#include "Malaria/LinearTCM.h"


ChangeTreatmentCoverageEvent::ChangeTreatmentCoverageEvent(ITreatmentCoverageModel* tcm) {
  time = tcm->starting_time;
  treatment_coverage_model = tcm;
  executable = true;
}

ChangeTreatmentCoverageEvent::~ChangeTreatmentCoverageEvent() {
  ObjectHelpers::delete_pointer(treatment_coverage_model);
}

void ChangeTreatmentCoverageEvent::execute() {
  Model::MODEL->set_treatment_coverage(treatment_coverage_model);

  if (auto* linear_tcm = dynamic_cast<LinearTCM*>(treatment_coverage_model)) {
    linear_tcm->update_rate_of_change();
  }
  std::cout << scheduler->current_time() << "-" << treatment_coverage_model->p_treatment_more_than_5[0] << std::endl;
  treatment_coverage_model = nullptr;
  executable = false;
}
