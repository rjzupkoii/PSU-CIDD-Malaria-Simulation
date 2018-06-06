/* 
 * File:   TestTreatmentFailureEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:36 AM
 */

#include "TestTreatmentFailureEvent.h"
#include "Person.h"
#include "ClonalParasitePopulation.h"
#include "Scheduler.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Model.h"
#include "Config.h"
#include "MDC/ModelDataCollector.h"

OBJECTPOOL_IMPL(TestTreatmentFailureEvent)

TestTreatmentFailureEvent::TestTreatmentFailureEvent() {
}

TestTreatmentFailureEvent::~TestTreatmentFailureEvent() {
    if (executable() && Model::DATA_COLLECTOR != nullptr) {
        Model::DATA_COLLECTOR-> number_of_treatments_with_therapy_ID()[therapyId_] -= 1;
    }
}

void TestTreatmentFailureEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time,  const int& t_id) {
    if (scheduler == nullptr) {
        std::cout << "error null" << std::endl;
        assert(false);
    }
    if (scheduler != nullptr) {
        TestTreatmentFailureEvent* e = new TestTreatmentFailureEvent();
        e->set_dispatcher(p);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        e->set_executable(true);
        e->set_time(time);
        e->set_therapyId(t_id);

        p->add(e);
        scheduler->schedule(e);
    }
}

void TestTreatmentFailureEvent::execute() {
    Person* person = (Person*) dispatcher();
    //    if (person->all_clonal_parasite_populations()->size() == 0) {
    //        //parasites might be cleaned by immune system or other things else
    //        //        if (person->is_tracking_treatment_number()) {
    //        //            Model::STATISTIC->record_1_treatment_success_by_therapy(person->last_therapy_id());
    //        //            person->set_is_tracking_treatment_number(false);
    //        //        }
    //
    //        if (!isResistance()) {
    //            Model::DATA_COLLECTOR->record_1_treatment_success_by_therapy(therapyId_);
    //            //            person->set_is_tracking_treatment_number(false);
    //        }
    //        return;
    //    } else {
    ////        if (person->has_detectable_parasite()) {
    ////            Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(person->location(), person->age(), therapyId_);
    ////        }
    //    }
    //
    //    bool tf = false;
    //    if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
    //        if (clinical_caused_parasite_->last_update_log10_parasite_density() >= Model::CONFIG->log_parasite_density_level().log_parasite_density_detectable) {
    //            //treatment failure
    //            tf = true;
    //            Model::DATA_COLLECTOR->record_1_TF(person->location(), true);
    //            if (!isResistance()) {
    //                Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(person->location(), person->age(), therapyId_);
    //            }
    //        }
    //    }
    //
    //    if (!tf) {
    //        if (!isResistance()) {
    //            Model::DATA_COLLECTOR->record_1_treatment_success_by_therapy(therapyId_);
    //            //            person->set_is_tracking_treatment_number(false);
    //        }
    //    }

    ///trial

    if (person->has_detectable_parasite()) {
        Model::DATA_COLLECTOR->record_1_TF(person->location(), true);
        Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(person->location(), person->age(), therapyId_);
    } else {
        Model::DATA_COLLECTOR->record_1_treatment_success_by_therapy(therapyId_);
    }
}
