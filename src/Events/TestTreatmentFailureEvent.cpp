/* 
 * File:   TestTreatmentFailureEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:36 AM
 */

#include "TestTreatmentFailureEvent.h"
#include "Population/Person.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Scheduler.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"

OBJECTPOOL_IMPL(TestTreatmentFailureEvent)

TestTreatmentFailureEvent::TestTreatmentFailureEvent() : clinical_caused_parasite_(nullptr), therapyId_(0) {}

TestTreatmentFailureEvent::~TestTreatmentFailureEvent() {
  if (executable && Model::DATA_COLLECTOR!=nullptr) {
    Model::DATA_COLLECTOR->number_of_treatments_with_therapy_ID()[therapyId_] -= 1;
  }
}

void TestTreatmentFailureEvent::schedule_event(Scheduler *scheduler, Person *p,
                                               ClonalParasitePopulation *clinical_caused_parasite,
                                               const int &time, const int &t_id) {
  if (scheduler==nullptr) {
    std::cout << "error null" << std::endl;
    assert(false);
  }
  if (scheduler!=nullptr) {
    auto *e = new TestTreatmentFailureEvent();
    e->dispatcher = p;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->time = time;
    e->set_therapyId(t_id);

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void TestTreatmentFailureEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);

  if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite())
      && clinical_caused_parasite_->last_update_log10_parasite_density() >
          Model::CONFIG->parasite_density_level().log_parasite_density_detectable) {

    Model::DATA_COLLECTOR->record_1_TF(person->location(), true);
    Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(person->location(), person->age_class(), therapyId_);
  }
}
