/* 
 * File:   Therapy.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 7:50 PM
 */

#ifndef THERAPY_H
#define	THERAPY_H

#include "PropertyMacro.h"
#include <vector>

class DrugType;

class Therapy {
    DISALLOW_COPY_AND_ASSIGN_(Therapy)

    VIRTUAL_PROPERTY_REF(int, id)
    VIRTUAL_PROPERTY_REF(int, number_of_dosing_days)
    VIRTUAL_PROPERTY_REF(int, testing_day)
    VIRTUAL_PROPERTY_REF(std::vector<int>, drug_ids);

public:
    Therapy();
    //    Therapy(const Therapy& orig);
    virtual ~Therapy();

    void add_drug(int drug_id);

    int get_arteminsinin_id();

//    int get_therapy_duration(int dosing_day);

private:

};

#endif	/* THERAPY_H */

