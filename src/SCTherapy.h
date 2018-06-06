/* 
 * File:   Therapy.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 7:50 PM
 */

#ifndef SCTHERAPY_H
#define	SCTHERAPY_H

#include "PropertyMacro.h"
#include "Therapy.h"
#include <vector>

class DrugType;

class SCTherapy: public Therapy {
    DISALLOW_COPY_AND_ASSIGN_(SCTherapy)

    
    VIRTUAL_PROPERTY_REF(int, artemisinin_id)
    VIRTUAL_PROPERTY_REF(int, dosing_day)
    VIRTUAL_PROPERTY_REF(std::vector<int>, drug_ids);

public:
    SCTherapy();
    //    Therapy(const Therapy& orig);
    virtual ~SCTherapy();

    void add_drug(int drug_id);

    int get_arteminsinin_id();

    //    int get_therapy_duration(int dosing_day);

private:

};

#endif	/* THERAPY_H */

