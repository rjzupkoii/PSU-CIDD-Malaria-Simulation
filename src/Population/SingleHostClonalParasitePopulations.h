/* 
 * File:   ParasitePopulation.h
 * Author: Merlin
 *
 * Created on July 11, 2013, 1:53 PM
 */

#ifndef SINGLEHOSTCLONALPARASITEPOPULATIONS_H
#define    SINGLEHOSTCLONALPARASITEPOPULATIONS_H

#include <vector>

#include "Core/PropertyMacro.h"
#include "Core/ObjectPool.h"
#include "Core/TypeDef.h"

class ClonalParasitePopulation;

class Person;

class DrugType;

class DrugsInBlood;

class ParasiteDensityUpdateFunction;

class SingleHostClonalParasitePopulations {
 OBJECTPOOL(SingleHostClonalParasitePopulations)

 DISALLOW_COPY_AND_ASSIGN(SingleHostClonalParasitePopulations)

 POINTER_PROPERTY(Person, person)

 POINTER_PROPERTY(std::vector<ClonalParasitePopulation *>, parasites)

 POINTER_PROPERTY(DoubleVector, relative_effective_parasite_density)

 PROPERTY_REF(double, log10_total_relative_density);

 private:
  int parasite_types = -1;

  void remove(const std::size_t &index);

 public:
  SingleHostClonalParasitePopulations(Person *person = nullptr);

  //    ParasitePopulation(const ParasitePopulation& orig);
  virtual ~SingleHostClonalParasitePopulations();

  void init();

  virtual int size();

  virtual void add(ClonalParasitePopulation *blood_parasite);

  virtual void add_all_infection_force();

  virtual void remove_all_infection_force();

  virtual void change_all_infection_force(const double &sign);

  virtual double get_log10_total_relative_density();

  virtual int latest_update_time() const;

  virtual bool contain(ClonalParasitePopulation *blood_parasite);

  void
  change_all_parasite_update_function(ParasiteDensityUpdateFunction *from, ParasiteDensityUpdateFunction *to) const;

  void update() const;

  void clear_cured_parasites();

  void clear();

  void update_by_drugs(DrugsInBlood *drugs_in_blood) const;

  bool has_detectable_parasite() const;

  void get_parasites_profiles(std::vector<double> &parasite_density, double &log10_total_relative_density) const;

  void update_relative_effective_parasite_density_using_free_recombination();

  void update_relative_effective_parasite_density_without_free_recombination();

  bool is_gametocytaemic() const;

};

#endif

