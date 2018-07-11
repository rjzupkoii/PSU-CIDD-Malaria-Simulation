/* 
 * File:   ExternalPopulation.h
 * Author: Merlin
 *
 * Created on August 21, 2013, 10:05 PM
 */

#ifndef EXTERNALPOPULATION_H
#define	EXTERNALPOPULATION_H
#include "Core/TypeDef.h"
#include "Core/PropertyMacro.h"
#include "Population.h"

class Model;
class PersonIndexAll;

class ExternalPopulation : public Population {
    DISALLOW_COPY_AND_ASSIGN(ExternalPopulation)
//    POINTER_PROPERTY(PersonIndexPtrList, person_index_list)
//    READ_ONLY_PROPERTY(PersonIndexAll*, all_persons)


public:
    ExternalPopulation(Model* model = nullptr);
    //    ExternalPopulation(const ExternalPopulation& orig);
    virtual ~ExternalPopulation();

    virtual void initialize();

    virtual void perform_infection_event();

    virtual void notify_change_in_force_of_infection(const int& location, const int& parasite_type_id, const double& relative_force_of_infection);

    virtual double get_current_daily_EIR_by_location(const int& current_time, const int& location, const int& parasite_type_id);
private:

};

#endif	/* EXTERNALPOPULATION_H */

