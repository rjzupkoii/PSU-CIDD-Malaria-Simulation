/*
 * DrugsInBlood.h
 *
 * Define the class that tracks the drugs that are currently in the blood.
 */
#ifndef DRUGSINBLOOD_H
#define DRUGSINBLOOD_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class Drug;
class DrugType;
class Person;

class DrugsInBlood {
 OBJECTPOOL(DrugsInBlood)

 DISALLOW_COPY_AND_ASSIGN(DrugsInBlood)

 DISALLOW_MOVE(DrugsInBlood)

 POINTER_PROPERTY(Person, person)

 POINTER_PROPERTY(DrugPtrMap, drugs)

 public:
  // Cutoff drugs that are less than or equal to 10%
  const double DRUG_CUTOFF_VALUE = 0.1;

  explicit DrugsInBlood(Person *person = nullptr);

  virtual ~DrugsInBlood();

  void init();

  Drug *add_drug(Drug *drug);

  bool is_drug_in_blood(DrugType *drug_type) const;

  [[nodiscard]] bool is_drug_in_blood(int drug_type_id) const;

  [[nodiscard]] Drug *get_drug(const int &type_id) const;

  [[nodiscard]] std::size_t size() const;

  void clear() const;

  void update() const;

  // Clear the drugs that were cut off.
  // This function was originally named clear_cut_off_drugs_by_event and took the calling Event object as a parameter.
  void clear_cut_off_drugs() const;

};

#endif
