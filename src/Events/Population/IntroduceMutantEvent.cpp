/**
 * IntroduceMutantEvent.cpp
 *
 * Implement the event.
 */
#include "IntroduceMutantEvent.h"

#include "Core/Scheduler.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "GIS/SpatialData.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

IntroduceMutantEvent::IntroduceMutantEvent(const int& time, const int& district, const double& fraction,
                                           const int& locus, const int& mutant_allele) :
  district_(district), fraction_(fraction), locus_(locus), mutant_allele_(mutant_allele) {
  this->time = time;
}

void IntroduceMutantEvent::execute() {
  // Calculate the target fraction of the district infections and perform them as needed
  auto locations = SpatialData::get_instance().get_district_locations(district_);
  double target_fraction = calculate(locations);
  auto count = (target_fraction > 0) ? mutate(locations, target_fraction) : 0;

  // Log the event's operation
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : Introduce mutant event, target fraction: "
    << target_fraction << ", mutations: " << count;
}

// Induce the mutations in individuals across the district, return the total number of mutations inflicted
int IntroduceMutantEvent::mutate(std::vector<int> &locations, double target_fraction) const {
  auto *pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

  auto mutations_count = 0;
  for (auto location : locations) {
    for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      // Note the infected individuals in the location
      auto infections = pi->vPerson()[location][Person::ASYMPTOMATIC][ac].size() + pi->vPerson()[location][Person::CLINICAL][ac].size();

      // Use a Poisson distribution to determine the number of mutations in this location
      auto mutations = Model::RANDOM->random_poisson((double)infections * target_fraction);
      if (mutations == 0) { continue; }
      mutations_count += mutations;

      // Note the number of asymptomatic cases for indexing operations
      auto asymptomatic = pi->vPerson()[location][Person::ASYMPTOMATIC][ac].size();

      // Perform the target number of mutations, operating on all infected individuals in the location and age class
      for (auto count = 0; count < mutations; count++) {
        // Use a uniform draw to get the index of an infected individual
        auto index = Model::RANDOM->random_uniform(infections);

        // Select the person based upon the index
        Person* person;
        if (index < asymptomatic) {
          person = pi->vPerson()[location][Person::ASYMPTOMATIC][ac][index];
        } else {
          person = pi->vPerson()[location][Person::CLINICAL][ac][index - asymptomatic];
        }

        // Mutate all the clonal populations the individual is carrying
        for (auto* pp : *(person->all_clonal_parasite_populations()->parasites())) {
          auto* old_genotype = pp->genotype();
          auto* new_genotype = old_genotype->combine_mutation_to(locus_, mutant_allele_);
          pp->set_genotype(new_genotype);
        }
      }
    }
  }

  // Return the total mutations count
  return mutations_count;
}

double IntroduceMutantEvent::calculate(std::vector<int> &locations) const {
  double mutant_fraction = 0.0;
  double parasite_population_count = 0;
  auto *pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

  // Calculate the frequency of the mutant type across the whole district
  for (auto location : locations) {
    for (auto hs : {Person::ASYMPTOMATIC, Person::CLINICAL}) {
      for (auto ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto &person : pi->vPerson()[location][hs][ac]) {
          parasite_population_count += person->all_clonal_parasite_populations()->size();
          for (auto &pp : *person->all_clonal_parasite_populations()->parasites()) {
            if (pp->genotype()->gene_expression()[locus_] == mutant_allele_) {
              mutant_fraction++;
            }
          }
        }
      }
    }
  }

  // Calculate and return the target fraction across the whole district
  mutant_fraction = (parasite_population_count == 0) ? 0 : mutant_fraction / parasite_population_count;
  return (fraction_ - mutant_fraction);
}
