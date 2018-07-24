/* 
 * File:   PersonIndexByLocationExternalPopulationMovingLevel.h
 * Author: Merlin
 *
 * Created on August 22, 2013, 12:31 PM
 */

#ifndef PERSONINDEXBYLOCATIONEXTERNALPOPULATIONMOVINGLEVEL_H
#define	PERSONINDEXBYLOCATIONEXTERNALPOPULATIONMOVINGLEVEL_H
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Person.h"
#include "PersonIndex.h"

class PersonIndexByLocationExternalPopulationMovingLevel: public PersonIndex {
    DISALLOW_COPY_AND_ASSIGN(PersonIndexByLocationExternalPopulationMovingLevel)
    PROPERTY_REF(PersonPtrVector3, vPerson)

public:
    PersonIndexByLocationExternalPopulationMovingLevel(const int& no_location = 1, const int& no_level = 1);
    //    PersonIndexByLocationExternalPopulationMovingLevel(const PersonIndexByLocationExternalPopulationMovingLevel& orig);
    virtual ~PersonIndexByLocationExternalPopulationMovingLevel();
    void Initialize(const int& no_location = 1, const int& no_level = 1);

    virtual void add(Person* p);

    virtual void remove(Person* p);

    virtual int size() const;

    virtual void update();

    virtual void notify_change(Person* p, const Person::Property& property, const void* oldValue, const void* newValue);

private:
    void remove_without_set_index(Person* p);
    void add(Person* p, const int& location, const int& moving_level);
    void change_property(Person* p, const int& location, const int& bitting_level);

};

#endif	/* PERSONINDEXBYLOCATIONEXTERNALPOPULATIONMOVINGLEVEL_H */

