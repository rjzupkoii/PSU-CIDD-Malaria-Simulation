/* 
 * File:   PersonIndexByLocationBittingLevel.h
 * Author: Merlin
 *
 * Created on July 3, 2013, 3:34 PM
 */

#ifndef PERSONINDEXBYLOCATIONBITTINGLEVEL_H
#define    PERSONINDEXBYLOCATIONBITTINGLEVEL_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Population/Person.h"
#include "PersonIndex.hxx"

class PersonIndexByLocationBittingLevel : public PersonIndex {
 DISALLOW_COPY_AND_ASSIGN(PersonIndexByLocationBittingLevel);
 PROPERTY_REF(PersonPtrVector3, vPerson);

 public:
  PersonIndexByLocationBittingLevel(const int &no_location = 1, const int &no_level = 1);

  //    PersonIndexByLocationBittingLevel(const PersonIndexByLocationBittingLevel& orig);
  virtual ~PersonIndexByLocationBittingLevel();

  void Initialize(const int &no_location = 1, const int &no_level = 1);

  virtual void add(Person *p);

  virtual void remove(Person *p);

  virtual std::size_t size() const;

  virtual void defragment();

  virtual void notify_change(Person *p, const Person::Property &property, const void *oldValue, const void *newValue);

 private:
  void remove_without_set_index(Person *p);

  void add(Person *p, const int &location, const int &bitting_level);

  void change_property(Person *p, const int &location, const int &bitting_level);
};

#endif    /* PERSONINDEXBYLOCATIONBITTINGLEVEL_H */

