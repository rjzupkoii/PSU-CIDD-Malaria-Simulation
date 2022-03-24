/* 
 * ColonalParasitePopulation.cpp
 * 
 * Implement the ColonalParasitePopulation class.
 */

#include "ClonalParasitePopulation.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Therapies/Therapy.hxx"
#include "Model.h"
#include "Core/Scheduler.h"
#include "Core/Config/Config.h"
#include "Helpers/NumberHelpers.h"
#include "Person.h"
#include <cmath>

OBJECTPOOL_IMPL(ClonalParasitePopulation)
const double ClonalParasitePopulation::LOG_ZERO_PARASITE_DENSITY = -1000;

ClonalParasitePopulation::ClonalParasitePopulation(Genotype *genotype) : 
  last_update_log10_parasite_density_(LOG_ZERO_PARASITE_DENSITY), 
  gametocyte_level_(0.0),
  first_date_in_blood_(-1),
  parasite_population_(nullptr),
  genotype_(genotype),
  update_function_(nullptr) {
    _uid = UniqueId::get_instance().get_uid();
  }

ClonalParasitePopulation::~ClonalParasitePopulation() = default;

double ClonalParasitePopulation::get_current_parasite_density(const int &current_time) {
  const auto duration = current_time - parasite_population()->latest_update_time();
  if (duration==0) {
    return last_update_log10_parasite_density_;
  }

  if (update_function_==nullptr) {
    //        std::cout << "hello" << std::endl;
    return last_update_log10_parasite_density_;
  }

  return update_function_->get_current_parasite_density(this, duration);
}

void ClonalParasitePopulation::mutate_to(Genotype *genotype) {
  //TODO:: do other statistic things
  set_genotype(genotype);
}

double ClonalParasitePopulation::get_log10_relative_density() const {

  if (NumberHelpers::is_equal(last_update_log10_parasite_density_, LOG_ZERO_PARASITE_DENSITY) ||
      NumberHelpers::is_equal(gametocyte_level_, 0.0))
    return LOG_ZERO_PARASITE_DENSITY;

  return last_update_log10_parasite_density_ + log10(gametocyte_level_);
}

double ClonalParasitePopulation::last_update_log10_parasite_density() const {
  return last_update_log10_parasite_density_;
}

void ClonalParasitePopulation::set_last_update_log10_parasite_density(const double &value) {
  if (NumberHelpers::is_enot_qual(last_update_log10_parasite_density_, value)) {
    parasite_population_->remove_all_infection_force();
    last_update_log10_parasite_density_ = value;
    parasite_population_->add_all_infection_force();
  }
}

double ClonalParasitePopulation::gametocyte_level() const {
  return gametocyte_level_;
}

void ClonalParasitePopulation::set_gametocyte_level(const double &value) {
  if (NumberHelpers::is_enot_qual(gametocyte_level_, value)) {
    parasite_population_->remove_all_infection_force();
    gametocyte_level_ = value;
    parasite_population_->add_all_infection_force();
  }
}

Genotype *ClonalParasitePopulation::genotype() const {

  return genotype_;
}

void ClonalParasitePopulation::set_genotype(Genotype *value) {
  if (genotype_!=value) {
    parasite_population_->remove_all_infection_force();
    genotype_ = value;
    parasite_population_->add_all_infection_force();
  }
}

bool ClonalParasitePopulation::resist_to(Therapy *therapy) const {
  return genotype_->resist_to(therapy);
}

bool ClonalParasitePopulation::resist_to(DrugType *dt) const {
  return genotype_->resist_to(dt);
}

bool ClonalParasitePopulation::resist_to(const int &drug_id) const {
  return genotype_->resist_to(Model::CONFIG->drug_db()->at(drug_id));
}

void ClonalParasitePopulation::update() {
  set_last_update_log10_parasite_density(get_current_parasite_density(Model::SCHEDULER->current_time()));
}

void ClonalParasitePopulation::perform_drug_action(const double &percent_parasite_remove) {


  double newSize = last_update_log10_parasite_density_;
  if (percent_parasite_remove > 1) {
    newSize = Model::CONFIG->parasite_density_level().log_parasite_density_cured;
  } else {
    newSize += log10(1 - percent_parasite_remove);
  }

  if (newSize < Model::CONFIG->parasite_density_level().log_parasite_density_cured) {
    newSize = Model::CONFIG->parasite_density_level().log_parasite_density_cured;
  }

//    std::cout << Model::SCHEDULER->current_time() << "\t" <<parasite_population()->person() << "\t"  << percent_parasite_remove << "\t"<<last_update_log10_parasite_density_ << "\t" <<newSize << std::endl;
  set_last_update_log10_parasite_density(newSize);
}
