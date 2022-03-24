/* 
 * ColonalParasitePopulation.h
 *
 * Define the ClonalParasitePopulation class which allows a single parasite 
 * colony to be tracked in an individual.
 */

#ifndef CLONALPARASITEPOPULATION_H
#define    CLONALPARASITEPOPULATION_H

#include "Core/PropertyMacro.h"
#include "ParasiteDensityUpdateFunction.h"
#include "Properties/IndexHandler.hxx"
#include "Therapies/DrugType.h"
#include "Core/ObjectPool.h"
#include "Helpers/UniqueId.hxx"

class Therapy;

class Genotype;

class SingleHostClonalParasitePopulations;

class ClonalParasitePopulation : public IndexHandler {
 OBJECTPOOL(ClonalParasitePopulation);

 DISALLOW_COPY_AND_ASSIGN(ClonalParasitePopulation)

 PROPERTY_HEADER(double, last_update_log10_parasite_density)

 PROPERTY_HEADER(double, gametocyte_level)

 PROPERTY_REF(int, first_date_in_blood)

 POINTER_PROPERTY(SingleHostClonalParasitePopulations, parasite_population)

 POINTER_PROPERTY_HEADER(Genotype, genotype)

 POINTER_PROPERTY(ParasiteDensityUpdateFunction, update_function)

 private:
  ul_uid _uid;

 public:
  static const double LOG_ZERO_PARASITE_DENSITY;

 public:
  ClonalParasitePopulation(Genotype *genotype = nullptr);

  virtual ~ClonalParasitePopulation();

  double get_current_parasite_density(const int &current_time);

  double get_log10_relative_density() const;

  void mutate_to(Genotype *genotype);

  bool resist_to(Therapy *therapy) const;

  bool resist_to(DrugType *dt) const;

  bool resist_to(const int &drug_id) const;

  void update();

  void perform_drug_action(const double &percent_parasite_remove);

  ul_uid get_uid() { return _uid; }

};

#endif

