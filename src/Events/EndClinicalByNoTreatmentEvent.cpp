/* 
 * File:   EndClinicalByNoTreatmentEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:28 PM
 */

#include "EndClinicalByNoTreatmentEvent.h"
#include "Population/Person.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Scheduler.h"
#include "Population/ImmuneSystem.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"

OBJECTPOOL_IMPL(EndClinicalByNoTreatmentEvent)

EndClinicalByNoTreatmentEvent::EndClinicalByNoTreatmentEvent() : clinical_caused_parasite_(nullptr) {}

EndClinicalByNoTreatmentEvent::~EndClinicalByNoTreatmentEvent() = default;

void EndClinicalByNoTreatmentEvent::schedule_event(Scheduler *scheduler, Person *p,
                                                   ClonalParasitePopulation *clinical_caused_parasite,
                                                   const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new EndClinicalByNoTreatmentEvent();
    e->dispatcher = p;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void EndClinicalByNoTreatmentEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);

  if (person->all_clonal_parasite_populations()->size()==0) {
    //        assert(false);
    person->change_state_when_no_parasite_in_blood();

  } else {
    //still have parasite in blood
    person->immune_system()->set_increase(true);
    person->set_host_state(Person::ASYMPTOMATIC);
    if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
      clinical_caused_parasite_->set_last_update_log10_parasite_density(
          Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic);

      person->determine_relapse_or_not(clinical_caused_parasite_);

    }
    //        std::cout << clinical_caused_parasite_->last_update_log10_parasite_density()<< std::endl;
    //        std::cout << person->immune_system()->get_lastest_immune_value()<< std::endl;
  }
}
