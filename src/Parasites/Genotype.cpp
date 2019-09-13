/* 
 * File:   Genotype.cpp
 * Author: Merlin
 * 
 * Created on March 17, 2014, 2:33 PM
 */

#include "Genotype.h"
#include "Therapies/DrugDatabase.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Therapies/SCTherapy.h"

Genotype::Genotype(const int &id, const GenotypeInfo &genotype_info, const IntVector &weight) : genotype_id_(id) {

  gene_expression_.clear();
  //
  auto v = id;
  for (auto i = 0; i < genotype_info.loci_vector.size(); i++) {
//    std::cout << v << "-" <<weight[i] << std::endl;
    gene_expression_.push_back(v/weight[i]);
    v = v%weight[i];
  }

  //daily_fitness
  daily_fitness_multiple_infection_ = 1;
  for (auto i = 0; i < genotype_info.loci_vector.size(); i++) {
    daily_fitness_multiple_infection_ *=
        1 - genotype_info.loci_vector[i].alleles[gene_expression_[i]].daily_cost_of_resistance;
  }

//    std::cout << id << "-" << daily_fitness_multiple_infection_<<std::endl;
  //number_of_resistance_position (level)
  number_of_resistance_position_ = 0;
  for (auto i = 0; i < genotype_info.loci_vector.size(); i++) {
    number_of_resistance_position_ += genotype_info.loci_vector[i].alleles[gene_expression_[i]].mutation_level;
  }

}

Genotype::~Genotype() = default;

bool Genotype::resist_to(DrugType* dt) {
  for (auto i = 0; i < dt->affecting_loci().size(); i++) {
    for (auto j = 0; j < dt->selecting_alleles()[i].size(); j++) {
      if (gene_expression_[dt->affecting_loci()[i]]==dt->selecting_alleles()[i][j]) {
        return true;
      }
    }
  }
  return false;
}

bool Genotype::resist_to(Therapy* therapy) {
  auto* sc_therapy = dynamic_cast<SCTherapy*> (therapy);
  if (sc_therapy!=nullptr) {
    for (auto drug_id : sc_therapy->drug_ids) {
      if (resist_to(Model::CONFIG->drug_db()->at(drug_id))) {
        return true;
      }
    }
  }
  return false;
}

Genotype* Genotype::combine_mutation_to(const int &locus, const int &value) {
  if (gene_expression_[locus]==value) {
    return this;
  }

  auto id = 0;
  for (auto i = 0; i < gene_expression_.size(); i++) {
    if (i==locus) {
      id += Model::CONFIG->genotype_db()->weight()[i]*value;
    } else {
      id += Model::CONFIG->genotype_db()->weight()[i]*gene_expression_[i];
    }
  }
  return Model::CONFIG->genotype_db()->at(id);
}

double Genotype::get_EC50_power_n(DrugType* dt) const {

  return get_EC50(dt->id());
}

double Genotype::get_EC50(const int &drug_id) const {

  return Model::CONFIG->EC50_power_n_table()[genotype_id_][drug_id];
}

int Genotype::select_mutation_allele(const int &mutation_locus) {
  const auto current_allele_value = gene_expression()[mutation_locus];

  //pos is from 0 to size -1
  const int pos = static_cast<const int>(Model::RANDOM->random_uniform_int(0,
                                                                           Model::CONFIG->genotype_info()
                                                                               .loci_vector[mutation_locus]
                                                                               .alleles[current_allele_value]
                                                                               .mutation_values.size()));
  //    double t = 1.0 / affecting_loci_.size();
  return Model::CONFIG->genotype_info().loci_vector[mutation_locus].alleles[current_allele_value].mutation_values[pos];
}

std::ostream &operator<<(std::ostream &os, const Genotype &e) {
  os << e.genotype_id_ << "\t";
  for (auto i = 0; i < e.gene_expression_.size(); i++) {
    const auto v = e.gene_expression_[i];
    os << Model::CONFIG->genotype_info().loci_vector[i].alleles[v];
  }

//    os << "\t" << e.number_of_resistance_position_;
  return os;
}