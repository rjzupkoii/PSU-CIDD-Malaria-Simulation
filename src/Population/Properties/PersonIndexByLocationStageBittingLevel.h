//
// Created by ktt5121 on 9/8/2021.
//

#ifndef POMS_PersonIndexByLocationStageBittingLevel_H
#define POMS_PersonIndexByLocationStageBittingLevel_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Population/Person.h"
#include "PersonIndex.hxx"

class PersonIndexByLocationStageBittingLevel : public PersonIndex  {
DISALLOW_COPY_AND_ASSIGN(PersonIndexByLocationStageBittingLevel)

PROPERTY_REF(PersonPtrVector4, vPerson);

public:
    //    PersonIndexByLocationStageBittingLevel();
    PersonIndexByLocationStageBittingLevel(const int &no_location = 1, const int &no_host_state = 1,
                                       const int &no_age_class = 1);

    //    PersonIndexByLocationStageBittingLevel(const PersonIndexByLocationStageBittingLevel& orig);
    virtual ~PersonIndexByLocationStageBittingLevel();

    void Initialize(const int &no_location = 1, const int &no_host_state = 1, const int &bitting_level = 1);

    virtual void add(Person *p);

    virtual void remove(Person *p);

    virtual std::size_t size() const;

    virtual void defragment();

    virtual void notify_change(Person *p, const Person::Property &property, const void *oldValue, const void *newValue);

private:
    void remove_without_set_index(Person *p);

    void add(Person *p, const int &location, const Person::HostStates &host_state, const int &bitting_level);

    void change_property(Person *p, const int &location, const Person::HostStates &host_state, const int &bitting_level);

};


#endif //POMS_PersonIndexByLocationStageBittingLevel_H
