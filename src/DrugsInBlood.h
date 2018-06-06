/* 
 * File:   DrugsInBlood.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 1:47 PM
 */

#ifndef DRUGSINBLOOD_H
#define	DRUGSINBLOOD_H
#include "PropertyMacro.h"
#include "TypeDef.h"
#include "ObjectPool.h"

class Person;
class Drug;
class Event;
class DrugType;

class DrugsInBlood {
    OBJECTPOOL(DrugsInBlood)
    DISALLOW_COPY_AND_ASSIGN_(DrugsInBlood)
    POINTER_PROPERTY(Person, person)
    POINTER_PROPERTY(DrugPtrMap, drugs)

public:
    DrugsInBlood(Person* person = nullptr);
    //    DrugsInBlood(const DrugsInBlood& orig);
    virtual ~DrugsInBlood();

    void init();
    
    Drug* add_drug(Drug* drug);

    bool is_drug_in_blood(DrugType* drugType);
    bool is_drug_in_blood(int drugTypeID);

    void remove_drug(Drug* drug);
    void remove_drug(const int& drugTypeID);

    Drug* get_drug(const int& typeID);

    int size();

    void clear(); 
    
    void update();

    void clear_cut_off_drugs_by_event(Event* event);
private:

};

#endif	/* DRUGSINBLOOD_H */

