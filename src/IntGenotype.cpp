/* 
 * File:   IntGenotype.cpp
 * Author: Merlin
 * 
 * Created on March 17, 2014, 2:33 PM
 */

#include "IntGenotype.h"
#include "DrugDatabase.h"
#include "Model.h"
#include "Config.h"
#include "Random.h"
#include "SCTherapy.h"

IntGenotype::IntGenotype(const int &id) : genotype_id_(id) {

    gene_expression_.clear();

    int v = id;
    for (int i = 0; i < Model::CONFIG->genotype_info().loci_vector.size(); i++) {
        gene_expression_.push_back(v / Model::CONFIG->genotype_db()->weight()[i]);
        v = v % Model::CONFIG->genotype_db()->weight()[i];
    }

    //daily_fitness
    daily_fitness_multiple_infection_ = 1;
    for (int i = 0; i < Model::CONFIG->genotype_info().loci_vector.size(); i++) {
        daily_fitness_multiple_infection_ *=
                1 - Model::CONFIG->genotype_info().loci_vector[i].alleles[gene_expression_[i]].daily_cost_of_resistance;
    }

//    std::cout << id << "-" << daily_fitness_multiple_infection_<<std::endl;
    //number_of_resistance_position (level)
    number_of_resistance_position_ = 0;
    for (int i = 0; i < Model::CONFIG->genotype_info().loci_vector.size(); i++) {
        number_of_resistance_position_ += Model::CONFIG->genotype_info().loci_vector[i].alleles[gene_expression_[i]].mutation_level;
    }

}

IntGenotype::~IntGenotype() {
}

bool IntGenotype::resist_to(DrugType *dt) {
    for (int i = 0; i < dt->affecting_loci().size(); i++) {

        for (int j = 0; j < dt->selecting_alleles()[i].size(); j++) {
            if (gene_expression_[dt->affecting_loci()[i]] == dt->selecting_alleles()[i][j]) {
                return true;
            }
        }
    }
    return false;
}

bool IntGenotype::resist_to(Therapy *therapy) {
    SCTherapy *scTherapy = dynamic_cast<SCTherapy *> (therapy);
    if (scTherapy != NULL) {
        for (int i = 0; i < scTherapy->drug_ids().size(); i++) {
            if (resist_to(Model::CONFIG->drug_db()->get(scTherapy->drug_ids()[i]))) {
                return true;
            }
        }
    }
    return false;
}

IntGenotype *IntGenotype::combine_mutation_to(const int &locus, const int &value) {
    if (gene_expression_[locus] == value) {
        return this;
    }

    int id = 0;
    for (int i = 0; i < gene_expression_.size(); i++) {
        if (i == locus) {
            id += Model::CONFIG->genotype_db()->weight()[i] * value;
        } else {
            id += Model::CONFIG->genotype_db()->weight()[i] * gene_expression_[i];
        }
    }
    return Model::CONFIG->genotype_db()->get(id);
}

double IntGenotype::get_EC50_power_n(DrugType *dt) {

    return get_EC50(dt->id());
}

double IntGenotype::get_EC50(const int &drug_id) {

    return Model::CONFIG->EC50_power_n_table()[genotype_id_][drug_id];
}

int IntGenotype::select_mutation_allele(const int &mutation_locus) {
    int current_allele_value = gene_expression()[mutation_locus];

    //pos is from 0 to size -1
    int pos = Model::RANDOM->random_uniform_int(0,
                                                Model::CONFIG->genotype_info().loci_vector[mutation_locus].alleles[current_allele_value].mutation_values.size());
    //    double t = 1.0 / affecting_loci_.size();
    return Model::CONFIG->genotype_info().loci_vector[mutation_locus].alleles[current_allele_value].mutation_values[pos];
}

std::ostream &operator<<(std::ostream &os, const IntGenotype &e) {
    os << e.genotype_id_ << "\t";
    for (int i = 0; i < e.gene_expression_.size(); i++) {
        int v = e.gene_expression_[i];
        os << Model::CONFIG->genotype_info().loci_vector[i].alleles[v];
    }

//    os << "\t" << e.number_of_resistance_position_;
    return os;
}