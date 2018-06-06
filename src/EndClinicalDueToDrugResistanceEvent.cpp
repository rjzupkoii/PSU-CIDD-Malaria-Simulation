/* 
 * File:   EndClinicalDueToDrugResistanceEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:24 AM
 */

#include "EndClinicalDueToDrugResistanceEvent.h"
#include "Scheduler.h"
#include "Person.h"
#include "ImmuneSystem.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Model.h"
#include "ClonalParasitePopulation.h"
#include "Config.h"

OBJECTPOOL_IMPL(EndClinicalDueToDrugResistanceEvent)


EndClinicalDueToDrugResistanceEvent::EndClinicalDueToDrugResistanceEvent() {
}

EndClinicalDueToDrugResistanceEvent::~EndClinicalDueToDrugResistanceEvent() {
}

void EndClinicalDueToDrugResistanceEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time) {
    if (scheduler != NULL) {
        EndClinicalDueToDrugResistanceEvent* e = new EndClinicalDueToDrugResistanceEvent();
        e->set_dispatcher(p);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void EndClinicalDueToDrugResistanceEvent::execute() {
    Person* person = (Person*) dispatcher();
    if (person->all_clonal_parasite_populations()->size() == 0) {
        person->change_state_when_no_parasite_in_blood();

    } else {
        //still have parasite in blood
        person->immune_system()->set_increase(true);
        person->set_host_state(Person::ASYMPTOMATIC);
        
        if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
            clinical_caused_parasite_->set_last_update_log10_parasite_density(Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic);

            person->determine_relapse_or_not(clinical_caused_parasite_);

        }
        
        //        person->determine_relapse_or_not(clinical_caused_parasite_);
        //        person->determine_clinical_or_not(clinical_caused_parasite_);

        //        if (clinical_caused_parasite_->last_update_log10_parasite_density() < Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic) {
        //            std::cout << clinical_caused_parasite_->last_update_log10_parasite_density()<< std::endl;
        //            assert(false);
        //        }
        //        clinical_caused_parasite_->set_last_update_log10_parasite_density(Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic);
        //        clinical_caused_parasite_->set_update_function(Model::MODEL->immunity_clearance_update_function());
        //        //        std::cout << "hello" << std::endl;
    }

}