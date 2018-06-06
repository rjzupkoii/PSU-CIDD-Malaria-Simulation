/* 
 * File:   PersonIndexByLocationBittingLevel.h
 * Author: Merlin
 *
 * Created on July 3, 2013, 3:34 PM
 */

#ifndef PERSONINDEXBYLOCATIONBITTINGLEVEL_H
#define	PERSONINDEXBYLOCATIONBITTINGLEVEL_H

#include "PropertyMacro.h"
#include "TypeDef.h"
#include "Person.h"
#include "PersonIndex.h"

class PersonIndexByLocationBittingLevel : public PersonIndex {
    DISALLOW_COPY_AND_ASSIGN_(PersonIndexByLocationBittingLevel);
    PROPERTY_REF(PersonPtrVector3, vPerson);

public:
    PersonIndexByLocationBittingLevel(const int& no_location = 1, const int& no_level = 1);
    //    PersonIndexByLocationBittingLevel(const PersonIndexByLocationBittingLevel& orig);
    virtual ~PersonIndexByLocationBittingLevel();

    void Initialize(const int& no_location = 1, const int& no_level = 1);

    virtual void add(Person* p);

    virtual void remove(Person* p);

    virtual int size() const;

    virtual void update();

    virtual void notify_change(Person* p, const Person::PersonProperties& property, const void* oldValue, const void* newValue);

private:
    void remove_without_set_index(Person* p);
    void add(Person* p, const int& location, const int& bitting_level);
    void change_property(Person* p, const int& location, const int& bitting_level);
};

#endif	/* PERSONINDEXBYLOCATIONBITTINGLEVEL_H */

