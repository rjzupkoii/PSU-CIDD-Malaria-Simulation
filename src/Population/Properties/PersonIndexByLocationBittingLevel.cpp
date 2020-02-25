/* 
 * File:   PersonIndexByLocationBittingLevel.cpp
 * Author: Merlin
 * 
 * Created on July 3, 2013, 3:34 PM
 */

#include "PersonIndexByLocationBittingLevel.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include <cassert>

PersonIndexByLocationBittingLevel::PersonIndexByLocationBittingLevel(const int &no_location, const int &no_level) {
  Initialize(no_location, no_level);
}

PersonIndexByLocationBittingLevel::~PersonIndexByLocationBittingLevel() {
}

void PersonIndexByLocationBittingLevel::Initialize(const int &no_location, const int &no_level) {
  vPerson_.clear();

  PersonPtrVector ppv;
  PersonPtrVector2 ppv2;
  ppv2.assign(no_level, ppv);

  vPerson_.assign(no_location, ppv2);
}

void PersonIndexByLocationBittingLevel::add(Person *p) {
  assert(vPerson_.size() > static_cast<std::size_t>(p->location()) && p->location() >= 0);
  assert(vPerson_[p->location()].size() > static_cast<std::size_t>(p->bitting_level()));
  add(p, p->location(), p->bitting_level());
}

void PersonIndexByLocationBittingLevel::remove(Person *p) {
  remove_without_set_index(p);
  p->PersonIndexByLocationBittingLevelHandler::set_index(-1);
}

void PersonIndexByLocationBittingLevel::notify_change(Person *p, const Person::Property &property, const void *oldValue,
                                                      const void *newValue) {
  switch (property) {
    case Person::LOCATION:change_property(p, *(int *) newValue, p->bitting_level());
      break;
    case Person::BITTING_LEVEL:change_property(p, p->location(), *(int *) newValue);
      break;
    default:

      break;
  }
}

std::size_t PersonIndexByLocationBittingLevel::size() const {
  return 0;
}

void PersonIndexByLocationBittingLevel::add(Person *p, const int &location, const int &bitting_level) {
  auto& reference = vPerson_[location][bitting_level];
  reference.push_back(p);
  p->PersonIndexByLocationBittingLevelHandler::set_index(vPerson_[location][bitting_level].size() - 1);
}

void PersonIndexByLocationBittingLevel::remove_without_set_index(Person *p) {
  auto & reference = vPerson_[p->location()][p->bitting_level()];
  reference.back()->PersonIndexByLocationBittingLevelHandler::set_index(p->PersonIndexByLocationBittingLevelHandler::index());
  reference[p->PersonIndexByLocationBittingLevelHandler::index()] = reference.back();
  reference.pop_back();
}

void PersonIndexByLocationBittingLevel::change_property(Person *p, const int &location, const int &bitting_level) {
  //remove from old position
  remove_without_set_index(p); //to save 1 set and improve performance since the index of p will changed when add

  //add to new position
  add(p, location, bitting_level);
}

void PersonIndexByLocationBittingLevel::defragment() {
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int bl = 0; bl < Model::CONFIG->relative_bitting_info().number_of_biting_levels; bl++) {
      std::vector<Person *>(vPerson_[location][bl]).swap(vPerson_[location][bl]);
    }
  }
}
