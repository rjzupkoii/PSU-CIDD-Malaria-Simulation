/* 
 * File:   Genotype.h
 * Author: Merlin
 *
 * Created on March 17, 2014, 2:33 PM
 */

#ifndef Genotype_H
#define Genotype_H

#include "Core/TypeDef.h"
#include "Core/PropertyMacro.h"

class DrugDatabase;

class DrugType;

class Therapy;

class Genotype {
 DISALLOW_COPY_AND_ASSIGN(Genotype)

 PROPERTY_REF(int, genotype_id)

 PROPERTY_REF(IntVector, gene_expression)

 PROPERTY_REF(double, daily_fitness_multiple_infection)

 PROPERTY_REF(int, number_of_resistance_position)

 POINTER_PROPERTY(DrugDatabase, drug_db)

 public:
  explicit Genotype(const int &id, const GenotypeInfo &genotype_info, const IntVector &weight);

  virtual ~Genotype();

  double get_EC50_power_n(DrugType *dt) const;

  double get_EC50(const int &drug_id) const;

  bool resist_to(DrugType *dt);

  bool resist_to(Therapy *therapy);

  Genotype *combine_mutation_to(const int &locus, const int &value);

  int select_mutation_allele(const int &mutation_locus);

  friend std::ostream &operator<<(std::ostream &os, const Genotype &e);

 private:

};

#endif /* Genotype_H */
