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

// Flat[x + WIDTH * (y + DEPTH * z)] 
#define lookup(flat, x, y, z, width, depth) flat[x + width * (y + depth * z)]

GenotypeDatabase::~GenotypeDatabase() {
  for (auto &i : *this) {
    delete i.second;
  }
  clear();
}

void GenotypeDatabase::add(Genotype *genotype) {
  if (this->find(genotype->genotype_id())!=this->end()) {
    delete (*this)[genotype->genotype_id()];
  }
  (*this)[genotype->genotype_id()] = genotype;
}

void GenotypeDatabase::initialize_matting_matrix() {
  const int size = static_cast<const int>(this->size());
  auto mating_matrix_ = MatingMatrix(size, std::vector<std::vector<double>>(size, std::vector<double>()));

  auto depth = 0;
  for (auto m = 0; m < size; m++) {
    for (auto f = 0; f < m; f++) {
      mating_matrix_[m][f] = generate_offspring_parasite_density((*this)[m]->gene_expression(), (*this)[f]->gene_expression());
      depth = (mating_matrix_[m][f].size() > depth) ? mating_matrix_[m][f].size() : depth;
    }
  }
  
  // Note the dimensions and allocate memory
  auto height = size;
  matrix_width = size;
  matrix_depth = depth;
  flat_matrix = new double[height * matrix_width * matrix_depth];

  // Now copy the values over
  for (auto ndx = 0; ndx < height; ndx++) {
    for (auto ndy = 0; ndy < matrix_width; ndy++) {
      for (auto ndz = 0; ndz < matrix_depth; ndz++) {
          lookup(flat_matrix, ndx, ndy, ndz, matrix_width, matrix_depth) = 
            mating_matrix_[ndx][ndy].size() != 0 ?  mating_matrix_[ndx][ndy][ndz] : 0;
      }
    }
  }
}

std::vector<double> GenotypeDatabase::generate_offspring_parasite_density(const IntVector &m, const IntVector &f) {
  std::vector<IntVector> results;
  //add first one
  const IntVector ge(m.size(), 0);
  results.push_back(ge);

  for (auto i = 0; i < m.size(); i++) {
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
  
  if (m < f) { return lookup(flat_matrix, f, m, p, matrix_width, matrix_depth); }
  return lookup(flat_matrix, m, f, p, matrix_width, matrix_depth);
}

int GenotypeDatabase::get_id(const IntVector &gene) {

  auto id = 0;
  for (auto i = 0; i < gene.size(); i++) {
    // locus i have weighted
    id += weight_[i]*gene[i];
  }

  return id;

}
