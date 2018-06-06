/* 
 * File:   BloodParasite.h
 * Author: Merlin
 *
 * Created on July 11, 2013, 2:21 PM
 */

#ifndef CLONALPARASITEPOPULATION_H
#define	CLONALPARASITEPOPULATION_H

#include "PropertyMacro.h"
#include "ParasiteDensityUpdateFunction.h"
#include "IndexHandler.h"
#include "DrugType.h"
#include "ObjectPool.h"

class Therapy;
class IntGenotype;
class SingleHostClonalParasitePopulations;

class ClonalParasitePopulation : public IndexHandler {
    OBJECTPOOL(ClonalParasitePopulation);
    DISALLOW_COPY_AND_ASSIGN_(ClonalParasitePopulation)

    PROPERTY_HEADER(double, last_update_log10_parasite_density)
    PROPERTY_HEADER(double, gametocyte_level)
//    PROPERTY_REF(double, clearance_rate)
    PROPERTY_REF(int, first_date_in_blood)

    POINTER_PROPERTY(SingleHostClonalParasitePopulations, parasite_population)
    //    PROPERTY(int, parasite_type_id);
    POINTER_PROPERTY_HEADER(IntGenotype, genotype)

    POINTER_PROPERTY(ParasiteDensityUpdateFunction, update_function)

public:
    static const double LOG_ZERO_PARASITE_DENSITY;

public:
    ClonalParasitePopulation(IntGenotype* genotype = nullptr);
    //    BloodParasite(const BloodParasite& orig);
    virtual ~ClonalParasitePopulation();

    double get_current_parasite_density(const int& current_time);

    double get_log10_relative_density();

    void mutate_to(IntGenotype* genotype);

    bool resist_to(Therapy* therapy);
    bool resist_to(DrugType* dt);
    bool resist_to(const int& drug_id);


    void update();

    void perform_drug_action(const double& percent_parasite_remove);

private:

};

#endif	/* CLONALPARASITEPOPULATION_H */

