/* 
 * File:   IntParasiteDatabase.cpp
 * Author: Merlin
 * 
 * Created on March 18, 2014, 3:06 PM
 */

#include "IntGenotypeDatabase.h"
#include "boost/foreach.hpp"
#include "IntGenotype.h"
#include "Model.h"
#include "Config.h"

IntGenotypeDatabase::IntGenotypeDatabase() {
    weight_.clear();
    weight_.assign(Model::CONFIG->genotype_info().loci_vector.size(), 1);

    int temp = 1;
    for (int i = weight_.size() - 2; i >-1; i--) {
        temp *= Model::CONFIG->genotype_info().loci_vector[i + 1].alleles.size();
        weight_[i] = temp;
    }
}

IntGenotypeDatabase::~IntGenotypeDatabase() {

    for(auto &i :  db_) {
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
    int size = db_.size();
    mating_matrix_ = MatingMatrix(size, std::vector< std::vector<double> > (size, std::vector<double>()));

    for (int m = 0; m < size; m++) {
        for (int f = 0; f < size; f++) {
            mating_matrix_[m][f] = generate_offspring_parasite_density(db_[m]->gene_expression(), db_[f]->gene_expression());
        }
    }
}

std::vector<double> IntGenotypeDatabase::generate_offspring_parasite_density(const IntVector& m, const IntVector& f) {
    std::vector < IntVector > results;
    //add first one
    IntVector ge(m.size(), 0);
    results.push_back(ge);


    for (int i = 0; i < m.size(); i++) {
        int old_size = results.size();
        for (int j = 0; j < old_size; j++) {
            results.push_back(results[j]);
            results[j][i] = m[i];
            results[results.size() - 1][i] = f[i];
        }
    }
    
    std::vector<double> recombination_parasite_density(db_.size(), 0);

    for(IntVector &ge_i :  results) {
        //        std::cout << ge_i << std::endl;
        recombination_parasite_density[get_id(ge_i)] += 1;
    }

    for (int i = 0; i < recombination_parasite_density.size(); i++) {
        if (recombination_parasite_density[i] != 0) {
            recombination_parasite_density[i] /= db_.size();
        }
    }

    return recombination_parasite_density;

}

int IntGenotypeDatabase::get_id(const IntVector& gene) {

    int id = 0;
    for (int i = 0; i < gene.size(); i++) {

        // locus i have weighted
        id += weight_[i] * gene[i];

    }

    return id;

}