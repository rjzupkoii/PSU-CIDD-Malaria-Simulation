/* 
 * File:   IntParasiteDatabase.cpp
 * Author: Merlin
 * 
 * Created on March 18, 2014, 3:06 PM
 */

#include "IntGenotypeDatabase.h"
#include "IntGenotype.h"
#include "Core/Config/Config.h"
#include "Helpers/NumberHelpers.h"

IntGenotypeDatabase::IntGenotypeDatabase() = default;

IntGenotypeDatabase::~IntGenotypeDatabase() {
  for (auto& i : db_) {
    delete i.second;
  }
  db_.clear();
}

void IntGenotypeDatabase::add(IntGenotype* genotype) {
  if (db_.find(genotype->genotype_id()) != db_.end()) {
    delete db_[genotype->genotype_id()];
  }
  db_[genotype->genotype_id()] = genotype;
}

IntGenotype* IntGenotypeDatabase::get(const int& id) {
  return db_.at(id);
}

void IntGenotypeDatabase::initialize_matting_matrix() {
  const int size = db_.size();
  mating_matrix_ = MatingMatrix(size, std::vector<std::vector<double>>(size, std::vector<double>()));

  for (auto m = 0; m < size; m++) {
    for (auto f = 0; f < size; f++) {
      mating_matrix_[m][f] = generate_offspring_parasite_density(db_[m]->gene_expression(), db_[f]->gene_expression());
    }
  }
}

std::vector<double> IntGenotypeDatabase::generate_offspring_parasite_density(const IntVector& m, const IntVector& f) {
  std::vector<IntVector> results;
  //add first one
  IntVector ge(m.size(), 0);
  results.push_back(ge);


  for (auto i = 0; i < m.size(); i++) {
    const int old_size = results.size();
    for (int j = 0; j < old_size; j++) {
      results.push_back(results[j]);
      results[j][i] = m[i];
      results[results.size() - 1][i] = f[i];
    }
  }

  std::vector<double> recombination_parasite_density(db_.size(), 0.0);

  for (auto& ge_i : results) {
    //        std::cout << ge_i << std::endl;
    recombination_parasite_density[get_id(ge_i)] += 1;
  }

  for (auto& density : recombination_parasite_density) {
    if (NumberHelpers::is_enot_qual(density, 0.0)) {
      density /= db_.size();
    }
  }

  return recombination_parasite_density;

}

int IntGenotypeDatabase::get_id(const IntVector& gene) {

  auto id = 0;
  for (auto i = 0; i < gene.size(); i++) {
    // locus i have weighted
    id += weight_[i] * gene[i];
  }

  return id;

}
