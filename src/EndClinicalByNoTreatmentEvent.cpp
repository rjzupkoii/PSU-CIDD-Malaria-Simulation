/* 
 * File:   EndClinicalByNoTreatmentEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:28 PM
 */

#include "EndClinicalByNoTreatmentEvent.h"
#include "Person.h"
#include "ClonalParasitePopulation.h"
#include "Scheduler.h"
#include "ImmuneSystem.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Model.h"
#include "Config.h"
#include "Random.h"

OBJECTPOOL_IMPL(EndClinicalByNoTreatmentEvent)

EndClinicalByNoTreatmentEvent::EndClinicalByNoTreatmentEvent() {
}

EndClinicalByNoTreatmentEvent::~EndClinicalByNoTreatmentEvent() {
}

void EndClinicalByNoTreatmentEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time) {
    if (scheduler != nullptr) {
        EndClinicalByNoTreatmentEvent* e = new EndClinicalByNoTreatmentEvent();
        e->set_dispatcher(p);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void EndClinicalByNoTreatmentEvent::execute() {

    Person* person = (Person*) dispatcher();

    if (person->all_clonal_parasite_populations()->size() == 0) {
//        assert(false);
        person->change_state_when_no_parasite_in_blood();

    } else {
        //still have parasite in blood
        person->immune_system()->set_increase(true);
        person->set_host_state(Person::ASYMPTOMATIC);
        if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
            clinical_caused_parasite_->set_last_update_log10_parasite_density(Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic);

            person->determine_relapse_or_not(clinical_caused_parasite_);
            
        }
        //        std::cout << clinical_caused_parasite_->last_update_log10_parasite_density()<< std::endl;
        //        std::cout << person->immune_system()->get_lastest_immune_value()<< std::endl;
    }
}