/* 
 * File:   ImmunityClearanceUpdateFunction.cpp
 * Author: Merlin
 * 
 * Created on July 29, 2013, 5:49 PM
 */

#include "ImmunityClearanceUpdateFunction.h"
#include "ClonalParasitePopulation.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Person.h"
#include "ImmuneSystem.h"
#include "Model.h"
#include "Parasites/Genotype.h"

ImmunityClearanceUpdateFunction::ImmunityClearanceUpdateFunction(Model *model) : model_(model) {}

ImmunityClearanceUpdateFunction::~ImmunityClearanceUpdateFunction() = default;

double ImmunityClearanceUpdateFunction::get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) {

  auto *p = parasite->parasite_population()->person();
  return p->immune_system()->get_parasite_size_after_t_days(duration, parasite->last_update_log10_parasite_density(),
                                                            parasite->genotype()->daily_fitness_multiple_infection());
}
