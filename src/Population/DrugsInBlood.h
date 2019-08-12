/* 
 * File:   DrugsInBlood.h
 * Author: Merlin
 *
 * Created on July 31, 2013, 1:47 PM
 */

#ifndef DRUGSINBLOOD_H
#define    DRUGSINBLOOD_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Core/ObjectPool.h"

class Person;

class Drug;

class Event;

class DrugType;

class DrugsInBlood {
 OBJECTPOOL(DrugsInBlood)

 DISALLOW_COPY_AND_ASSIGN(DrugsInBlood)

 DISALLOW_MOVE(DrugsInBlood)

 POINTER_PROPERTY(Person, person)

 POINTER_PROPERTY(DrugPtrMap, drugs)

 public:
  explicit DrugsInBlood(Person *person = nullptr);

  //    DrugsInBlood(const DrugsInBlood& orig);
  virtual ~DrugsInBlood();

  void init();

  Drug *add_drug(Drug *drug);

  bool is_drug_in_blood(DrugType *drug_type) const;

  bool is_drug_in_blood(int drug_type_id) const;

  void remove_drug(Drug *drug) const;

  void remove_drug(const int &drug_type_id) const;

  Drug *get_drug(const int &type_id) const;

  std::size_t size() const;

  void clear() const;

  void update() const;

  void clear_cut_off_drugs_by_event(Event *event) const;

};

#endif    /* DRUGSINBLOOD_H */
