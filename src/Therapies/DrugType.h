/* 
 * File:   DrugType.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 10:59 AM
 */

#ifndef DRUGTYPE_H
#define DRUGTYPE_H

#include "Core/TypeDef.h"
#include "Core/PropertyMacro.h"

typedef std::map<std::string, double> ec50map_type;

class Genotype;

class DrugType {
public:
DISALLOW_COPY_AND_ASSIGN(DrugType)

VIRTUAL_PROPERTY_REF(int, id)

VIRTUAL_PROPERTY_REF(std::string, name)

VIRTUAL_PROPERTY_REF(double, drug_half_life)

VIRTUAL_PROPERTY_REF(double, maximum_parasite_killing_rate)

VIRTUAL_PROPERTY_REF(DoubleVector, age_group_specific_drug_concentration_sd);

VIRTUAL_PROPERTY_REF(DoubleVector, age_specific_drug_absorption);

VIRTUAL_PROPERTY_REF(double, p_mutation)

VIRTUAL_PROPERTY_REF(double, k)

VIRTUAL_PROPERTY_REF(double, cut_off_percent)

  // TODO: Consider removing effecting_loci and selecting alleles
VIRTUAL_PROPERTY_REF(IntVector, affecting_loci)

VIRTUAL_PROPERTY_REF(IntVector2, selecting_alleles)

VIRTUAL_PROPERTY_REF(ec50map_type, ec50_map)

public:
  DrugType();

  virtual ~DrugType() = default;

  virtual double get_parasite_killing_rate_by_concentration(const double &concentration, const double &EC50_power_n);

  virtual double n() { return n_; }

  virtual void set_n(const double &n) { n_ = n; }

  int get_total_duration_of_drug_activity(const int &dosing_days) const;

  int select_mutation_locus();

  double infer_ec50(Genotype* genotype);

private:
  double n_;
};

#endif 
