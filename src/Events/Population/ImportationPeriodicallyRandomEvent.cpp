/*
 * ImportationPeriodicallyRandomEvent.cpp
 * 
 * Implement the event.
 */
#include "ImportationPeriodicallyRandomEvent.h"

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Core/Scheduler.h"
#include "Helpers/TimeHelpers.h"
#include "Model.h"
#include "Population/ImmuneSystem.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

void ImportationPeriodicallyRandomEvent::execute() {

  // Start by finding the number of infections to inflict
  auto date = static_cast<date::year_month_day>(Model::SCHEDULER->calendar_date);
  auto days = TimeHelpers::days_in_month(static_cast<int>(date.year()), static_cast<unsigned int>(date.month()));
  auto infections = Model::RANDOM->random_poisson((double)count_ / days);

  // Inflict the infections
  for (auto ndx = 0; ndx < infections; ndx++) {
    // Get the location and person index
    auto location = get_location();
    auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

    // Get the age classes for the susceptible individuals
    unsigned long age_class;
    do {
      age_class = Model::RANDOM->random_uniform(static_cast<unsigned long>(pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE].size()));
    } while (pi->vPerson()[location][0][age_class].empty()) ;

    // Get the individual
    unsigned long index = Model::RANDOM->random_uniform(pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class].size());
    auto* person = pi->vPerson()[location][Person::HostStates::SUSCEPTIBLE][age_class][index];

    // Inflict the infection
    infect(person, genotypeId_);

    // Log on demand
    VLOG(1) << date << " - Introduced infection at " << location;
  }

  // Minimum schedule update is one day
  auto time = Model::SCHEDULER->current_time() + 1;
  if (static_cast<unsigned int>(date.day()) == days) {
    // If it is the last day of the month, schedule for the first of the month, next year
    auto nextRun = date::year_month_day(date.year() + date::years{1}, date.month(), date::day{1});
    time = (date::sys_days{nextRun} - date::sys_days{Model::CONFIG->starting_date()}).count();
  }

  // Generate and schedule the event
  auto* event = new ImportationPeriodicallyRandomEvent(genotypeId_, time, count_, log_parasite_density_);
  scheduler->schedule_population_event(event);
}

// The following is based upon a fitness proportionate selection (roulette wheel selection) 
std::size_t ImportationPeriodicallyRandomEvent::get_location() {

  // Note the common values
  auto locations = Model::CONFIG->number_of_locations();
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = pi->vPerson()[0][Person::HostStates::SUSCEPTIBLE].size();

  // Start by finding the total susceptible population
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

void ImportationPeriodicallyRandomEvent::infect(Person* person, int genotypeId) const {
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

  // Check if the configured log density is equal to or greater than the standard for clinical
  if (log_parasite_density_ >= Model::CONFIG->parasite_density_level().log_parasite_density_clinical) {
    blood_parasite->set_update_function(Model::MODEL->progress_to_clinical_update_function());
    person->schedule_progress_to_clinical_event_by(blood_parasite);
  }
}