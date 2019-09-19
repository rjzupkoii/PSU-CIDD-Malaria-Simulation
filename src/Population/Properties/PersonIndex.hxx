/* 
 * File:   PersonIndex.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:01 AM
 */

#ifndef PERSONINDEX_H
#define    PERSONINDEX_H

#include "Core/PropertyMacro.h"
#include "Population/Person.h"

class PersonIndex {
 DISALLOW_COPY_AND_ASSIGN(PersonIndex)

 public:
  PersonIndex() = default;

  virtual ~PersonIndex() = default;

  virtual void add(Person *p) = 0;

  virtual void remove(Person *p) = 0;

  virtual std::size_t size() const = 0;

  virtual void defragment() = 0;

  virtual void
  notify_change(Person *p, const Person::Property &property, const void *oldValue, const void *newValue) = 0;

 private:

};

#endif

