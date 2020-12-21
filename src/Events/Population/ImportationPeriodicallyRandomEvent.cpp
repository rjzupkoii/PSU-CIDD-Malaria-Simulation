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
#include "Population/Properties/PersonIndexAll.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

void ImportationPeriodicallyRandomEvent::execute() {
  // Get the location and person index
  int location = get_location();
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

  // Get the age classes for the suspetable individuals
  unsigned long age_class = 0;
  do {
    age_class = Model::RANDOM->random_uniform(static_cast<unsigned long>(pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE].size()));
  } while (pi->vPerson()[location][0][age_class].empty()) ;

  // Get the individual
  unsigned long index = Model::RANDOM->random_uniform(pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class].size());
  auto* person = pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class][index];

  // Inflict the infection
  infect(person, genotypeId_);

  // Schedule the next time to run
  auto time = scheduler->current_time() + periodicity_;
  ImportationPeriodicallyRandomEvent* event = new ImportationPeriodicallyRandomEvent(genotypeId_, time, periodicity_, log_parasite_density_);
  scheduler->schedule_population_event(event);

  // Log on demand
  VLOG(1) << "Introduced infection at " << location;
}

// The following is based upon a fitness proportionate selection (roulette wheel selection) 
int ImportationPeriodicallyRandomEvent::get_location() {

  // Note the common values
  auto locations = Model::CONFIG->number_of_locations();  
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = pi->vPerson()[0][Person::HostStates::SUSCEPTIBLE].size();

  // Start by finding the total suspetable population
  auto population = 0ul;
  for (std::size_t location = 0; location < locations; location++) {
    for (std::size_t age_class = 0; age_class < age_classes; age_class++) {
      population += pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class].size();
    }
  }

  // Get a random pull [0, population - 1]
  auto target = Model::RANDOM->random_uniform_int(0, population - 1);

  // Find the index of location which has a cumulative sum less than the target value
  for (std::size_t location = 0; location < locations; location++) {
    // Note we have to check for each age class since we don't know exactly how
    // many individuals are in a given location otherwise
    for (std::size_t age_class = 0; age_class < age_classes; age_class++) {
      if (target < pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class].size()) {
        return location;
      }
      target -= pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class].size();
    }
  }

  // If all goes well we shouldn't reach this point
  LOG(ERROR) << "Failed to return index with a remainder of " << target;
  throw std::runtime_error("Failed to find a random location for genotype import");
}

void ImportationPeriodicallyRandomEvent::infect(Person* person, int genotypeId) {
  // Prepare the immune system    
  person->immune_system()->set_increase(true);
  person->set_host_state(Person::ASYMPTOMATIC);

  // Prepare the genotype
  Genotype* genotype = Model::CONFIG->genotype_db()->at(genotypeId);

  // Inflict the infection
  auto* blood_parasite = person->add_new_parasite_to_blood(genotype);
  blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
  blood_parasite->set_last_update_log10_parasite_density(log_parasite_density_);
  blood_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());
}