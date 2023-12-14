/* 
 * SingleHostClonalParasitePopulations.cpp
 *
 * Implement the single host container for clonal parasite populations.
 */
#include "ClonalParasitePopulation.h"

#include <cmath>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "DrugsInBlood.h"
#include "Helpers/NumberHelpers.hxx"
#include "Helpers/ObjectHelpers.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Parasites/Genotype.h"
#include "Person.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Therapies/Drug.h"

OBJECTPOOL_IMPL(SingleHostClonalParasitePopulations)

SingleHostClonalParasitePopulations::SingleHostClonalParasitePopulations(Person* person) : person_(person),
                                                                                           parasites_(nullptr),
                                                                                           relative_effective_parasite_density_(nullptr),
                                                                                           log10_total_relative_density_(ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY) { }

void SingleHostClonalParasitePopulations::init() {
  parasites_ = new std::vector<ClonalParasitePopulation*>();
  if (Model::CONFIG != nullptr) {
    parasite_types = static_cast<int>(Model::CONFIG->number_of_parasite_types());
    relative_effective_parasite_density_ = new DoubleVector(parasite_types, 0.0);
  }
}

SingleHostClonalParasitePopulations::~SingleHostClonalParasitePopulations() {
  if (parasites_ != nullptr) {
    clear();
    ObjectHelpers::delete_pointer<std::vector<ClonalParasitePopulation*>>(parasites_);
  }

  ObjectHelpers::delete_pointer<std::vector<double>>(relative_effective_parasite_density_);

  person_ = nullptr;

}

void SingleHostClonalParasitePopulations::clear() {
  if (parasites_->empty()) return;
  remove_all_infection_force();

  for (auto &parasite : *parasites_) {
    delete parasite;
  }
  parasites_->clear();

}

void SingleHostClonalParasitePopulations::add(ClonalParasitePopulation* blood_parasite) {
  blood_parasite->set_parasite_population(this);

  parasites_->push_back(blood_parasite);
  blood_parasite->set_index(parasites_->size() - 1);
  assert(parasites_->at(blood_parasite->index()) == blood_parasite);
}

// Remove the parasite at the given index from the population, does not recalculate the infection force.
void SingleHostClonalParasitePopulations::remove(const std::size_t &index) {
  ClonalParasitePopulation* bp = parasites_->at(index);
  if (bp->index() != index) {
    std::cout << bp->index() << "-" << index << "-" << parasites_->at(index)->index() << std::endl;
    assert(bp->index() == index);
  }

  parasites_->back()->set_index(index);
  parasites_->at(index) = parasites_->back();
  parasites_->pop_back();
  bp->set_index(-1);

  bp->set_parasite_population(nullptr);

  delete bp;
}

void SingleHostClonalParasitePopulations::remove_all_infection_force() {
  change_all_infection_force(-1);
}

void SingleHostClonalParasitePopulations::add_all_infection_force() {
  //update relative_effective_parasite_density_
  if (Model::CONFIG->using_free_recombination()) {
    update_relative_effective_parasite_density_using_free_recombination();
  } else {
    update_relative_effective_parasite_density_without_free_recombination();
  }

  change_all_infection_force(1);
}

void SingleHostClonalParasitePopulations::change_all_infection_force(const double &sign) {
  // Return if there isn't a person
  if (person_ == nullptr) { return; }

  // Return if the density is zero
  if (NumberHelpers::is_equal(log10_total_relative_density_, ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY)) { return; }

  // Note that this is not as safe as ::at() but is considerably faster since we avoid range checking!
  for (auto p = 0; p < parasite_types; p++) {    
      person_->notify_change_in_force_of_infection(sign, p, (*relative_effective_parasite_density_)[p], log10_total_relative_density_);
  }
}

void SingleHostClonalParasitePopulations::update_relative_effective_parasite_density_without_free_recombination() {
  // Get the parasite profiles, if the density is zero then return
  std::vector<double> relative_parasite_density(size(), 0.0);
  get_parasites_profiles(relative_parasite_density, log10_total_relative_density_);
  if (NumberHelpers::is_equal(log10_total_relative_density_, ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY)) { return; }

  // Zero the current values
  for (auto p = 0; p < parasite_types; p++) {
    (*relative_effective_parasite_density_)[p] = 0.0;
  }

  // Update the current values
  for (std::size_t i = 0; i < relative_parasite_density.size(); i++) {
    if (NumberHelpers::is_zero(relative_parasite_density[i])) { continue; }
    auto index = (*parasites_)[i]->genotype()->genotype_id();
    (*relative_effective_parasite_density_)[index] += relative_parasite_density[i];
  }
}

void SingleHostClonalParasitePopulations::update_relative_effective_parasite_density_using_free_recombination() {
  // Get the population count, if it is zero then set the density and return
  std::size_t parasite_population_count = size();
  if (parasite_population_count == 0) {
    log10_total_relative_density_ = ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY;
    return;
  }

  // Get the parasite profiles, if the density is zero then return
  std::vector<double> relative_parasite_density(parasite_population_count, 0.0);
  get_parasites_profiles(relative_parasite_density, log10_total_relative_density_);
  if (NumberHelpers::is_equal(log10_total_relative_density_, ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY)) { return; }

  // Assert that nothing was deleted in the process
  assert(relative_parasite_density.size() == parasite_population_count);

  // Start by zeroing out current parasite densities
  std::fill((*relative_effective_parasite_density_).begin(), (*relative_effective_parasite_density_).end(), 0);

  // Cache the genotype DB reference
  const auto& genotype_db = Model::CONFIG->genotype_db();

  for (std::size_t i = 0; i < parasite_population_count; i++) {
    const auto density_i = relative_parasite_density[i];
    if (NumberHelpers::is_zero(density_i)) { continue; }

    for (std::size_t j = i; j < parasite_population_count; j++) {
      const auto density_j = relative_parasite_density[j];
      if (NumberHelpers::is_zero(density_j)) { continue; }

      // Are they the same?
      if (i == j) {
        const auto weight = density_i * density_i;
        const auto index = (*parasites_)[i]->genotype()->genotype_id();
        (*relative_effective_parasite_density_)[index] += weight;
        continue;
      }

      // Different, more complicated update
      const auto weight = 2 * density_i * density_j;
      const auto id_f = (*parasites_)[i]->genotype()->genotype_id();
      const auto id_m = (*parasites_)[j]->genotype()->genotype_id();
      for (auto p = 0; p < parasite_types; p++) {
        // Check to see if a call to get_offspring_density is needed, if the indices are the same then we know the value
        // is zero (no change), or one (update with weight)
        if (id_f == id_m) {
          if (id_f == p) {
            (*relative_effective_parasite_density_)[p] += weight;
          }
        } else {
          // Check the density matrix and use that to ensure that the right weight is applied to the update
          auto offspring_density = (id_f < id_m) ? genotype_db->mating_matrix->get(id_m, id_f, p) : genotype_db->mating_matrix->get(id_f, id_m, p);
          (*relative_effective_parasite_density_)[p] += weight * offspring_density;
        }
      }
    }
  }
}

void SingleHostClonalParasitePopulations::get_parasites_profiles(std::vector<double> &relative_parasite_density, double &log10_total_relative_density) const {
  
  // Note the size once
  std::size_t size = parasites_->size();

  // Scan the densities to see if they are all zero
  std::size_t i = 0;
  while ((i < size) && (NumberHelpers::is_equal((*parasites_)[i]->get_log10_relative_density(), ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY))) {
    relative_parasite_density[i] = 0.0;
    i++;
  }

  // If all densities are zero, then the total must be zero
  if (i == size) {
    log10_total_relative_density = ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY;
    return;
  }

  // Some parasites are still present, continue the calculation
  log10_total_relative_density = (*parasites_)[i]->get_log10_relative_density();
  relative_parasite_density[i] = (log10_total_relative_density);

  // Scan the remainder of the parasites
  for (std::size_t j = i + 1; j < size; j++) {
    const auto log10_relative_density = (*parasites_)[j]->get_log10_relative_density();

    // Update or clear accordingly
    if (NumberHelpers::is_not_equal(log10_relative_density, ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY)) {
      relative_parasite_density[j] = (log10_relative_density);
      log10_total_relative_density += log10(pow(10, log10_relative_density - log10_total_relative_density) + 1);
    } else {
      relative_parasite_density[j] = (0.0);
    }
  }

  // Update the densities
  for (std::size_t j = 0; j < size; j++) {
    if (NumberHelpers::is_not_zero(relative_parasite_density[j])) {
      relative_parasite_density[j] = pow(10, relative_parasite_density[j] - log10_total_relative_density);
    }
  }

}

int SingleHostClonalParasitePopulations::latest_update_time() const {
  return person_->latest_update_time();
}

int SingleHostClonalParasitePopulations::size() {
  return static_cast<int>(parasites_->size());
}

bool SingleHostClonalParasitePopulations::contain(ClonalParasitePopulation* blood_parasite) {

  for (auto &parasite : *parasites_) {
    if (blood_parasite == parasite) {
      return true;
    }
  }

  return false;
}

void SingleHostClonalParasitePopulations::change_all_parasite_update_function(ParasiteDensityUpdateFunction* from,
                                                                              ParasiteDensityUpdateFunction* to) const {
  for (auto* parasite : *parasites_) {
    if (parasite->update_function() == from) {
      parasite->set_update_function(to);
    }
  }

}

void SingleHostClonalParasitePopulations::update() const {
  for (auto* bp : *parasites_) {
    bp->update();
  }
}

void SingleHostClonalParasitePopulations::clear_cured_parasites() {
  // Only recalculate on update
  bool updated = false;

  // Clear all the cured parasites from the individual
  for (int i = static_cast<int>(parasites_->size()) - 1; i >= 0; i--) {
    if ((*parasites_)[i]->last_update_log10_parasite_density() <= Model::CONFIG->parasite_density_level().log_parasite_density_cured + 0.00001) {
      // Clear the infection force prior to removal
      if (!updated) {
        remove_all_infection_force();
        updated = true;
     }
      remove(i);
    }
  }

  // Update the infection force
  if (updated) {
    add_all_infection_force();
  }
}

void SingleHostClonalParasitePopulations::update_by_drugs(DrugsInBlood* drugs_in_blood) const {
  for (auto &blood_parasite : *parasites_) {
    // Create a pointer to the current genotype
    auto* new_genotype = blood_parasite->genotype();

    // Percentage to remove is originally zero
    double percent_parasite_remove = 0.0;

    for (auto &index : *drugs_in_blood->drugs()) {
      const auto drug = index.second;

      // Roll the dice to see if a mutation occurred
      const auto p = Model::RANDOM->random_flat(0.0, 1.0);
      if (p < drug->get_mutation_probability()) {
        // A mutation may occur, first we need to determine what it might be
        auto mutation_locus = static_cast<int>(Model::RANDOM->random_uniform_int(0, new_genotype->gene_expression().size()));
        auto mutation_allele = blood_parasite->genotype()->select_mutation_allele(mutation_locus);
        auto* mutation_genotype = new_genotype->combine_mutation_to(mutation_locus, mutation_allele);

        // Next, we need to determine if the mutation is adventitious to the parasite. We do this by checking the EC50
        // and if the EC50 is higher than the current genotype, the drug is less efficacious and the mutation is favorable
        // to the parasite. As a modeling simplification we ignore scenarios where the EC50 is less than since the drug
        // is likely to clear the parasite.
        if (mutation_genotype->get_EC50_power_n(drug->drug_type()) > new_genotype->get_EC50_power_n(drug->drug_type())) {
          new_genotype = mutation_genotype;
        }
      }

      // If the new genotype is not the same as the old one, then a mutation occurred
      if (new_genotype != blood_parasite->genotype()) {
        Model::DATA_COLLECTOR->record_1_mutation(person_->location());
        blood_parasite->set_genotype(new_genotype);
      }

      // Update the percentage 
      const auto p_temp = drug->get_parasite_killing_rate(blood_parasite->genotype()->genotype_id());
      percent_parasite_remove = (percent_parasite_remove + p_temp) - (percent_parasite_remove * p_temp);
    }

    // If the percent to remove is greater than zero, then perform the drug action
    if (percent_parasite_remove > 0) {
      blood_parasite->perform_drug_action(percent_parasite_remove);
    }
  }
}

bool SingleHostClonalParasitePopulations::has_detectable_parasite() const {
  for (auto &parasite : *parasites_) {
    if (parasite->last_update_log10_parasite_density() >=
        Model::CONFIG->parasite_density_level().log_parasite_density_detectable_pfpr) {
      return true;
    }
  }
  return false;
}

// Check to see if any of the parasites are gametocytaemic
bool SingleHostClonalParasitePopulations::is_gametocytaemic() const {
  // This approach to the code is slightly faster than a foreach iterator 
  // which is important since this gets called a lot!
  auto size = parasites_->size();
  for (std::size_t ndx = 0; ndx < size; ndx++) {
    if ((*parasites_)[ndx]->gametocyte_level() > 0) {
      return true;
    }
  }
  return false;
}
