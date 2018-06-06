/* 
 * File:   DrugType.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 10:59 AM
 */

#ifndef DRUGTYPE_H
#define DRUGTYPE_H
#include "TypeDef.h"
#include "PropertyMacro.h"
#include <boost/dynamic_bitset.hpp>

typedef std::map<std::string, double> ec50map_type;

class IntGenotype;

class DrugType {
public:

    enum DrugFamily {
        Artemisinin = 0,
        Lumefantrine = 1,
        Amodiaquine = 2,
        Piperaquine = 3,
        Mefloquine = 4,
        SP = 5,
        Other
    };
    DISALLOW_COPY_AND_ASSIGN_(DrugType)
    VIRTUAL_PROPERTY_REF(int, id)

    VIRTUAL_PROPERTY_REF(DrugFamily, drug_family)
    VIRTUAL_PROPERTY_REF(double, drug_half_life)
    VIRTUAL_PROPERTY_REF(double, maximum_parasite_killing_rate)
    VIRTUAL_PROPERTY_REF(DoubleVector, age_group_specific_drug_concentration_sd);

    VIRTUAL_PROPERTY_REF(double, p_mutation)
    VIRTUAL_PROPERTY_REF(double, k)
    VIRTUAL_PROPERTY_REF(double, cut_off_percent)

// TODO: Consider removing effecting_loci and selecting alleles
    VIRTUAL_PROPERTY_REF(IntVector, affecting_loci)
    VIRTUAL_PROPERTY_REF(IntVector2, selecting_alleles)
    VIRTUAL_PROPERTY_REF(ec50map_type, ec50map)


public:
    DrugType();

    virtual ~DrugType();

    void reset(int length);

    virtual double get_parasite_killing_rate_by_concentration(const double &concentration, const double& EC50_power_n );

    virtual double n();
    virtual void set_n(const double& n);

    int get_total_duration_of_drug_activity(const int &dosing_days);

    bool is_artemisinin();
    bool is_lumefantrine();

    int select_mutation_locus();

    double inferEC50(IntGenotype* genotype);

private:
    double n_;
    //    double EC50_;
};

#endif /* DRUGTYPE_H */

