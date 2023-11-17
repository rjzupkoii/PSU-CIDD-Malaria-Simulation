/* 
 * PersonIndexByLocationBitingLevel.h
 *
 * Define the person index by location and biting level.
 */
#ifndef PERSON_INDEX_BY_LOCATION_BITING_LEVEL_H
#define PERSON_INDEX_BY_LOCATION_BITING_LEVEL_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "PersonIndex.hxx"
#include "Population/Person.h"

class PersonIndexByLocationBitingLevel : public PersonIndex {
 DISALLOW_COPY_AND_ASSIGN(PersonIndexByLocationBitingLevel);
 PROPERTY_REF(PersonPtrVector3, vPerson);

 public:
  explicit PersonIndexByLocationBitingLevel(const int &no_location = 1, const int &no_level = 1);

  ~PersonIndexByLocationBitingLevel() override = default;

  void Initialize(const int &no_location = 1, const int &no_level = 1);

  void add(Person *p) override;

  void remove(Person *p) override;

  [[nodiscard]] std::size_t size() const override;

  void defragment() override;

  void notify_change(Person *p, const Person::Property &property, const void *oldValue, const void *newValue) override;

 private:
  void remove_without_set_index(Person *p);

  void add(Person *p, const int &location, const int &biting_level);

  void change_property(Person *p, const int &location, const int &biting_level);
};

#endif

