/* 
 * File:   MoveParasiteToBloodEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 11:14 PM
 */

#include "MoveParasiteToBloodEvent.h"
#include "Population/Person.h"
#include "Population/ImmuneSystem.h"
#include "Model.h"
#include "Core/Random.h"
#include "Core/Config/Config.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/DrugsInBlood.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Core/Scheduler.h"

OBJECTPOOL_IMPL(MoveParasiteToBloodEvent)

MoveParasiteToBloodEvent::MoveParasiteToBloodEvent() : infection_genotype_(nullptr) {}

MoveParasiteToBloodEvent::~MoveParasiteToBloodEvent() {}

void
MoveParasiteToBloodEvent::schedule_event(Scheduler *scheduler, Person *p, Genotype *infection_type, const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new MoveParasiteToBloodEvent();
    e->dispatcher = p;
    e->set_infection_genotype(infection_type);
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void MoveParasiteToBloodEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  auto *parasite_type = person->liver_parasite_type();
  person->set_liver_parasite_type(nullptr);

  //add to blood
  if (person->host_state()==Person::EXPOSED) {
    person->set_host_state(Person::ASYMPTOMATIC);
  }

  person->immune_system()->set_increase(true);

  auto new_parasite = person->add_new_parasite_to_blood(parasite_type);

  new_parasite->set_last_update_log10_parasite_density(
      Model::RANDOM->random_normal_truncated(
          Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic, 0.5));

  if (person->has_effective_drug_in_blood()) {
    //person has drug in blood
    new_parasite->set_update_function(Model::MODEL->having_drug_update_function());
  } else {

    if (person->all_clonal_parasite_populations()->size() > 1) {
      if (Model::CONFIG->allow_new_coinfection_to_cause_symtoms()) {
        person->determine_clinical_or_not(new_parasite);
      } else {
        new_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());
      }
    } else {
      person->determine_clinical_or_not(new_parasite);
    }

  }

  person->schedule_mature_gametocyte_event(new_parasite);
}
