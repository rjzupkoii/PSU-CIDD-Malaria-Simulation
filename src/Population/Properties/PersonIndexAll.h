/* 
 * File:   PersonIndexAll.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:15 AM
 */

#ifndef PERSONINDEXALL_H
#define    PERSONINDEXALL_H

#include "PersonIndex.hxx"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class PersonIndexAll : public PersonIndex {
 DISALLOW_COPY_AND_ASSIGN(PersonIndexAll)

 PROPERTY_REF(PersonPtrVector, vPerson)

 public:
  PersonIndexAll();

  virtual ~PersonIndexAll();

  virtual void add(Person *p);

  virtual void remove(Person *p);

  virtual std::size_t size() const;

  virtual void defragment();

  virtual void notify_change(Person *p, const Person::Property &property, const void *oldValue, const void *newValue);

 private:

};

#endif    /* PERSONINDEXALL_H */

