/* 
 * File:   UpdateByHavingDrugEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:16 PM
 */

#include "UpdateWhenDrugIsPresentEvent.h"

#include "Population/Person.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Scheduler.h"
#include "Population/DrugsInBlood.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Core/Config/Config.h"
#include "Model.h"

OBJECTPOOL_IMPL(UpdateWhenDrugIsPresentEvent)

UpdateWhenDrugIsPresentEvent::UpdateWhenDrugIsPresentEvent() : clinical_caused_parasite_(nullptr) {}

UpdateWhenDrugIsPresentEvent::~UpdateWhenDrugIsPresentEvent() = default;

void UpdateWhenDrugIsPresentEvent::schedule_event(Scheduler *scheduler, Person *p,
                                                  ClonalParasitePopulation *clinical_caused_parasite, const int &time) {
  if (scheduler!=nullptr) {

    auto *e = new UpdateWhenDrugIsPresentEvent();
    e->dispatcher = p;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void UpdateWhenDrugIsPresentEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  if (person->drugs_in_blood()->size() > 0) {
    if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_) && person->host_state()==
        Person::CLINICAL) {
      if (clinical_caused_parasite_->last_update_log10_parasite_density() <= Model::CONFIG
          ->parasite_density_level().
          log_parasite_density_asymptomatic) {
        person->set_host_state(Person::ASYMPTOMATIC);
      }
    }
    person->schedule_update_by_drug_event(clinical_caused_parasite_);
  } else {
    //        no drug in blood, reset the status of drug Update parasite
    // the drug update parasite is inserted into blood when  there is still drug in blood and also the clinical cause parasite

    for (auto i = 0; i < person->all_clonal_parasite_populations()->size(); i++) {
      const auto blood_parasite = person->all_clonal_parasite_populations()->parasites()->at(i);
      if (blood_parasite->update_function()==Model::MODEL->having_drug_update_function()) {
        person->determine_relapse_or_not(blood_parasite);
      }
    }
  }
}
