/* 
 * File:   ParasiteDatabase.cpp
 * Author: Merlin
 * 
 * Created on March 18, 2014, 3:06 PM
 */
#include "GenotypeDatabase.h"

#include "Genotype.h"
#include "Core/Config/Config.h"
#include "Helpers/NumberHelpers.h"

GenotypeDatabase::~GenotypeDatabase() {
  for (auto &i : *this) {
    delete i.second;
  }
  clear();
  delete mating_matrix;
}

void GenotypeDatabase::add(Genotype *genotype) {
  if (this->find(genotype->genotype_id())!=this->end()) {
    delete (*this)[genotype->genotype_id()];
  }
  (*this)[genotype->genotype_id()] = genotype;
}

void GenotypeDatabase::initialize_matting_matrix() {
  const int size = static_cast<const int>(this->size());
  auto working = MatingMatrix(size, std::vector<std::vector<double>>(size, std::vector<double>()));

  for (auto m = 0; m < size; m++) {
    for (auto f = 0; f < m; f++) {
      working[m][f] = generate_offspring_parasite_density((*this)[m]->gene_expression(), (*this)[f]->gene_expression());
    }
  }
  
  mating_matrix = Flat3D<double>::flatten(working);
}

std::vector<double> GenotypeDatabase::generate_offspring_parasite_density(const IntVector &m, const IntVector &f) {
  std::vector<IntVector> results;
  //add first one
  const IntVector ge(m.size(), 0);
  results.push_back(ge);

  for (std::size_t i = 0; i < m.size(); i++) {
    const int old_size = static_cast<const int>(results.size());
    for (auto j = 0; j < old_size; j++) {
      results.push_back(results[j]);
      results[j][i] = m[i];
      results[results.size() - 1][i] = f[i];
    }
  }

  std::vector<double> recombination_parasite_density(this->size(), 0.0);

  for (auto &ge_i : results) {
    //        std::cout << ge_i << std::endl;
    recombination_parasite_density[get_id(ge_i)] += 1;
  }

  for (auto &density : recombination_parasite_density) {
    if (NumberHelpers::is_enot_qual(density, 0.0)) {
      density /= results.size();
    }
  }

  return recombination_parasite_density;

}

// Get the offspring density from the flat matrix.
double GenotypeDatabase::get_offspring_density(const int &m, const int &f, const int &p) {
  if (m == f) {
    return (f == p) ? 1 : 0;
  }

  if (m < f) { return mating_matrix->get(f, m, p); }
  return mating_matrix->get(m, f, p);
}

int GenotypeDatabase::get_id(const IntVector &gene) {

  auto id = 0;
  for (std::size_t i = 0; i < gene.size(); i++) {
    // locus i have weighted
    id += weight_[i]*gene[i];
  }

  return id;

}
