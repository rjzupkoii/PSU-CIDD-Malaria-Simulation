/*
 * ImportationPeriodicallyRandomEvent.cpp
 * 
 * Implement the event.
 */
#include "ImportationPeriodicallyRandomEvent.h"

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Core/Scheduler.h"
#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

void ImportationPeriodicallyRandomEvent::execute() {
  // Get the location and person index
  int location = get_location();
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

  // Get the age class
  unsigned long age_class = 0;
  do {
    age_class = Model::RANDOM->random_uniform(static_cast<unsigned long>(pi->vPerson()[location][0].size()));
  } while (pi->vPerson()[location][0][age_class].empty()) ;

  // Get the individual
  unsigned long index = Model::RANDOM->random_uniform(pi->vPerson()[location][0][age_class].size());
  auto* person = pi->vPerson()[location][0][age_class][index];

  // Inflict the infection
  infect(person, genotypeId_);

  // Schedule the next time to run
  auto time = scheduler->current_time() + periodicity_;
  ImportationPeriodicallyRandomEvent* event = new ImportationPeriodicallyRandomEvent(genotypeId_, time, periodicity_);
  scheduler->schedule_population_event(event);

  // Log on demand
  VLOG(1) << "Introduced infection at " << location;
}

int ImportationPeriodicallyRandomEvent::get_location() {

    // TODO Finish this function

}

void ImportationPeriodicallyRandomEvent::infect(Person* person, int genotypeId) {
  // Prepare the immune system    
  person->immune_system()->set_increase(true);
  person->set_host_state(Person::ASYMPTOMATIC);

  // Prepare the genotype
  Genotype* genotype = Model::CONFIG->genotype_db()->at(genotypeId);

  // Inflict the infection
  auto* blood_parasite = person->add_new_parasite_to_blood(genotype);
  auto size = Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic;
  blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
  blood_parasite->set_last_update_log10_parasite_density(size);
  blood_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());
}