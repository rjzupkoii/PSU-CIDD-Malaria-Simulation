/* 
 * ImmunityClearanceUpdateFunction.cpp
 *
 * Implement the parasite clearance due to the immune response.
 */
#include "ImmunityClearanceUpdateFunction.h"

#include "Model.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/ImmuneSystem.h"
#include "Population/Person.h"

ImmunityClearanceUpdateFunction::ImmunityClearanceUpdateFunction(Model *model) : model_(model) { }

double ImmunityClearanceUpdateFunction::get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) {
  auto *p = parasite->parasite_population()->person();
  return p->immune_system()->get_parasite_size_after_t_days(duration, parasite->last_update_log10_parasite_density(),
                                                            parasite->genotype()->daily_fitness_multiple_infection());
}
