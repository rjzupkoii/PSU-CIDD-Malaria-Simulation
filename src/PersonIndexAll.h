/* 
 * File:   PersonIndexAll.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:15 AM
 */

#ifndef PERSONINDEXALL_H
#define	PERSONINDEXALL_H

#include "PersonIndex.h"
#include "PropertyMacro.h"
#include "TypeDef.h"

class PersonIndexAll: public PersonIndex {
    DISALLOW_COPY_AND_ASSIGN_(PersonIndexAll)
    
    PROPERTY_REF(PersonPtrVector, vPerson)
    
public:
    PersonIndexAll();    
    virtual ~PersonIndexAll();
    
    virtual void add(Person* p);
    virtual void remove(Person* p);
    virtual int size() const;
    virtual void update();
    
    virtual void notify_change(Person* p, const Person::PersonProperties& property, const void* oldValue, const void* newValue);
    
    
private:

};

#endif	/* PERSONINDEXALL_H */

