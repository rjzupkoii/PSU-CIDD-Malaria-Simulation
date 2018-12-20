/* 
 * File:   IntParasiteDatabase.h
 * Author: Merlin
 *
 * Created on March 18, 2014, 3:06 PM
 */

#ifndef INTPARASITEDATABASE_H
#define    INTPARASITEDATABASE_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Genotype.h"
#include <map>

class Genotype;

typedef std::map<ul, Genotype*> GenotypePtrMap;
typedef std::vector<std::vector<std::vector<double>>> MatingMatrix;

class GenotypeDatabase : public GenotypePtrMap {
 DISALLOW_COPY_AND_ASSIGN(GenotypeDatabase)

 DISALLOW_MOVE(GenotypeDatabase)

 VIRTUAL_PROPERTY_REF(MatingMatrix, mating_matrix)

 VIRTUAL_PROPERTY_REF(IntVector, weight)

 public:
  GenotypeDatabase();

  virtual ~GenotypeDatabase();

  void add(Genotype* genotype);

  int get_id(const IntVector &gene);

  void initialize_matting_matrix();

  std::vector<double> generate_offspring_parasite_density(const IntVector &m, const IntVector &f);

  double get_offspring_density(const int &m, const int &f, const int &p);

 private:

};

#endif    /* INTPARASITEDATABASE_H */
