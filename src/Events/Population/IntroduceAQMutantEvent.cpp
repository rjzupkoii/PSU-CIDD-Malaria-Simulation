#include "IntroduceAQMutantEvent.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Model.h"
#include "Population/Population.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Population/SingleHostClonalParasitePopulations.h"

IntroduceAQMutantEvent::IntroduceAQMutantEvent(const int& location, const int& execute_at, const double& fraction) :
  location_(location),
  fraction_(fraction) {
  time = execute_at;
}

IntroduceAQMutantEvent::~IntroduceAQMutantEvent() = default;

void IntroduceAQMutantEvent::execute() {
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();


  for (std::size_t j = 0; j < Model::CONFIG->number_of_age_classes(); ++j) {
    static const std::vector<int> mutant_alleles{0, 1, 3, 4, 5, 7};


    const auto number_infected_individual_in_ac =
      pi->vPerson()[0][Person::ASYMPTOMATIC][j].size() + pi->vPerson()[0][Person::CLINICAL][j].size();
    const auto number_of_importation_cases = Model::RANDOM->random_poisson(
      number_infected_individual_in_ac * fraction_);
    if (number_of_importation_cases == 0)
      continue;
    for (auto i = 0; i < number_of_importation_cases; i++) {

      const auto index = Model::RANDOM->random_uniform(number_infected_individual_in_ac);

      Person* p = nullptr;
      if (index < pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()) {
        p = pi->vPerson()[0][Person::ASYMPTOMATIC][j][index];
      }
      else {
        p = pi->vPerson()[0][Person::CLINICAL][j][index - pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()];
      }

      for (auto* pp : *(p->all_clonal_parasite_populations()->parasites())) {
        auto* old_genotype = pp->genotype();

        auto* new_genotype = old_genotype->combine_mutation_to(0, 1);
        // 0, 1,3,4,5,7
        const auto allele_index = Model::RANDOM->random_uniform(mutant_alleles.size());
        new_genotype = new_genotype->combine_mutation_to(1, mutant_alleles[allele_index]);
        pp->set_genotype(new_genotype);
      }
    }
  }

  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : " << this->name();
}
