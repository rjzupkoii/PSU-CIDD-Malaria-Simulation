/* 
 * File:   IntParasiteDatabase.h
 * Author: Merlin
 *
 * Created on March 18, 2014, 3:06 PM
 */

#ifndef INTPARASITEDATABASE_H
#define	INTPARASITEDATABASE_H
#include "PropertyMacro.h"
#include "TypeDef.h"
#include "IntGenotype.h"
//#include <boost/dynamic_bitset.hpp>
#include <map>

class IntGenotype;

typedef std::map<int, IntGenotype*> IntGenotypePtrMap;
typedef std::vector< std::vector < std::vector <double> > > MatingMatrix;

class IntGenotypeDatabase {
    DISALLOW_COPY_AND_ASSIGN_(IntGenotypeDatabase)
    VIRTUAL_PROPERTY_REF(IntGenotypePtrMap, db);
    VIRTUAL_PROPERTY_REF(MatingMatrix, mating_matrix);
    VIRTUAL_PROPERTY_REF(IntVector, weight);


public:
    IntGenotypeDatabase();
    virtual ~IntGenotypeDatabase();

    void add(IntGenotype* genotype);
    IntGenotype* get(const int& id);

    int get_id(const IntVector& gene);

    void initialize_matting_matrix();

    std::vector<double> generate_offspring_parasite_density(const IntVector& m, const IntVector& f);
private:

};




#endif	/* INTPARASITEDATABASE_H */

