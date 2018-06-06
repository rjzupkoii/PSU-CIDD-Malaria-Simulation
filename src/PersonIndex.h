/* 
 * File:   PersonIndex.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:01 AM
 */

#ifndef PERSONINDEX_H
#define	PERSONINDEX_H
#include "PropertyMacro.h"
#include "Person.h"

class PersonIndex {
    DISALLOW_COPY_AND_ASSIGN_(PersonIndex)

public:
    PersonIndex();
    virtual ~PersonIndex();


    virtual void add(Person* p) = 0;
    virtual void remove(Person* p) = 0;
    virtual int size() const = 0;
    virtual void update() = 0;

    virtual void notify_change(Person* p, const Person::PersonProperties& property, const void* oldValue, const void* newValue) = 0;
private:

};

#endif	/* PERSONINDEX_H */

