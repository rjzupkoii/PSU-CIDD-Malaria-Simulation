/* 
 * File:   PersonIndexByLocationMovingLevel.h
 * Author: Merlin
 *
 * Created on August 1, 2013, 9:26 PM
 */

#ifndef PERSONINDEXBYLOCATIONMOVINGLEVEL_H
#define	PERSONINDEXBYLOCATIONMOVINGLEVEL_H
#include "PropertyMacro.h"
#include "TypeDef.h"
#include "Person.h"
#include "PersonIndex.h"


class PersonIndexByLocationMovingLevel : public PersonIndex {
    DISALLOW_COPY_AND_ASSIGN_(PersonIndexByLocationMovingLevel);
    PROPERTY_REF(PersonPtrVector3, vPerson);
public:
    PersonIndexByLocationMovingLevel(const int& no_location = 1, const int& no_level = 1);
    //    PersonIndexByLocationMovingLevel(const PersonIndexByLocationMovingLevel& orig);
    virtual ~PersonIndexByLocationMovingLevel();
    void Initialize(const int& no_location = 1, const int& no_level = 1);

    virtual void add(Person* p);

    virtual void remove(Person* p);

    virtual int size() const;
    
    virtual void update();

    virtual void notify_change(Person* p, const Person::PersonProperties& property, const void* oldValue, const void* newValue);

private:
    void remove_without_set_index(Person* p);
    void add(Person* p, const int& location, const int& moving_level);
    void change_property(Person* p, const int& location, const int& bitting_level);

};

#endif	/* PERSONINDEXBYLOCATIONMOVINGLEVEL_H */

