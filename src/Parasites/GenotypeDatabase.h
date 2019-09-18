/* 
 * File:   ParasiteDatabase.h
 * Author: Merlin
 *
 * Created on March 18, 2014, 3:06 PM
 */

#ifndef PARASITEDATABASE_H
#define PARASITEDATABASE_H

#include "Core/FlatArray.hxx"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Genotype.h"
#include <map>
#include "Population/SingleHostClonalParasitePopulations.h"

class Genotype;

typedef std::map<ul, Genotype*> GenotypePtrMap;
typedef std::vector<std::vector<std::vector<double>>> MatingMatrix;

class GenotypeDatabase : public GenotypePtrMap {
  DISALLOW_COPY_AND_ASSIGN(GenotypeDatabase)

  DISALLOW_MOVE(GenotypeDatabase)

  VIRTUAL_PROPERTY_REF(IntVector, weight)

  private:
    Flat3D<double>* mating_matrix;

    // This is generally bad practice, but since the method gets called a lot 
    // during the model we need to get as much performance as we can out of it.
    // By making the function a friend we can avoid any stack calls at the
    // expense of needing to be more diligent about our code.
    friend void SingleHostClonalParasitePopulations::update_relative_effective_parasite_density_using_free_recombination();

  public:
    GenotypeDatabase() = default;

    virtual ~GenotypeDatabase();

    void add(Genotype* genotype);

    int get_id(const IntVector &gene);

    void initialize_matting_matrix();

    std::vector<double> generate_offspring_parasite_density(const IntVector &m, const IntVector &f);

    double get_offspring_density(const int &m, const int &f, const int &p);
};

#endif
