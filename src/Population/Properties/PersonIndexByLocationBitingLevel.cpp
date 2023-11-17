/* 
 * PersonIndexByLocationBitingLevel.cpp
 *
 * Implement the person index by location and biting level.
 */
#include "PersonIndexByLocationBitingLevel.h"

#include "Core/Config/Config.h"
#include "Model.h"

PersonIndexByLocationBitingLevel::PersonIndexByLocationBitingLevel(const int &no_location, const int &no_level) {
  Initialize(no_location, no_level);
}

void PersonIndexByLocationBitingLevel::Initialize(const int &no_location, const int &no_level) {
  vPerson_.clear();

  PersonPtrVector ppv;
  PersonPtrVector2 ppv2;
  ppv2.assign(no_level, ppv);

  vPerson_.assign(no_location, ppv2);
}

void PersonIndexByLocationBitingLevel::add(Person *p) {
  assert(vPerson_.size() > static_cast<std::size_t>(p->location()) && p->location() >= 0);
  assert(vPerson_[p->location()].size() > static_cast<std::size_t>(p->biting_level()));
  add(p, p->location(), p->biting_level());
}

void PersonIndexByLocationBitingLevel::remove(Person *p) {
  remove_without_set_index(p);
  p->PersonIndexByLocationBitingLevelHandler::set_index(-1);
}

void PersonIndexByLocationBitingLevel::notify_change(Person *p, const Person::Property &property, const void *oldValue,
                                                     const void *newValue) {
  switch (property) {
    case Person::LOCATION:change_property(p, *(int *) newValue, p->biting_level());
      break;
    case Person::BITING_LEVEL:change_property(p, p->location(), *(int *) newValue);
      break;
    default:

      break;
  }
}

std::size_t PersonIndexByLocationBitingLevel::size() const {
  return 0;
}

void PersonIndexByLocationBitingLevel::add(Person *p, const int &location, const int &biting_level) {
  auto& reference = vPerson_[location][biting_level];
  reference.push_back(p);
  p->PersonIndexByLocationBitingLevelHandler::set_index(vPerson_[location][biting_level].size() - 1);
}

void PersonIndexByLocationBitingLevel::remove_without_set_index(Person *p) {
  auto & reference = vPerson_[p->location()][p->biting_level()];
  reference.back()->PersonIndexByLocationBitingLevelHandler::set_index(p->PersonIndexByLocationBitingLevelHandler::index());
  reference[p->PersonIndexByLocationBitingLevelHandler::index()] = reference.back();
  reference.pop_back();
}

void PersonIndexByLocationBitingLevel::change_property(Person *p, const int &location, const int &biting_level) {
  //remove from old position
  remove_without_set_index(p); //to save 1 set and improve performance since the index of p will be changed when add

  //add to new position
  add(p, location, biting_level);
}

void PersonIndexByLocationBitingLevel::defragment() {
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int bl = 0; bl < Model::CONFIG->relative_bitting_info().number_of_biting_levels; bl++) {
      std::vector<Person *>(vPerson_[location][bl]).swap(vPerson_[location][bl]);
    }
  }
}
