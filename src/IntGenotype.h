/* 
 * File:   IntGenotype.h
 * Author: Merlin
 *
 * Created on March 17, 2014, 2:33 PM
 */

#ifndef INTGENOTYPE_H
#define INTGENOTYPE_H
#include "TypeDef.h"
#include "PropertyMacro.h"

class DrugDatabase;
class DrugType;
class Therapy;

class IntGenotype {
    DISALLOW_COPY_AND_ASSIGN_(IntGenotype)
    PROPERTY_REF(int, genotype_id);
    PROPERTY_REF(IntVector, gene_expression);
    PROPERTY_REF(double, daily_fitness_multiple_infection);
    PROPERTY_REF(int, number_of_resistance_position);
    POINTER_PROPERTY(DrugDatabase, drug_db)
public:
    IntGenotype(const int& id = 0);

    virtual ~IntGenotype();

    double get_EC50_power_n(DrugType *dt);
    double get_EC50(const int &drug_id);

    bool resist_to(DrugType* dt);

    bool resist_to(Therapy* therapy);

    IntGenotype* combine_mutation_to(const int &locus, const int &value);

    int select_mutation_allele(const int & mutation_locus);

    friend std::ostream& operator<<(std::ostream& os, const IntGenotype& e);

private:

};

#endif /* INTGENOTYPE_H */

