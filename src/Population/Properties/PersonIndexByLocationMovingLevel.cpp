/* 
* File:   PersonIndexByLocationMovingLevel.cpp
* Author: Merlin
* 
* Created on August 1, 2013, 9:26 PM
*/

#include "PersonIndexByLocationMovingLevel.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include <cassert>

PersonIndexByLocationMovingLevel::PersonIndexByLocationMovingLevel(const int &no_location, const int &no_level) {
  Initialize(no_location, no_level);
}

PersonIndexByLocationMovingLevel::~PersonIndexByLocationMovingLevel() {
  vPerson_.clear();
}

void PersonIndexByLocationMovingLevel::Initialize(const int &no_location, const int &no_level) {
  vPerson_.clear();

  PersonPtrVector ppv;
  PersonPtrVector2 ppv2;
  ppv2.assign(no_level, ppv);

  vPerson_.assign(no_location, ppv2);
}

void PersonIndexByLocationMovingLevel::add(Person *p) {
  assert(vPerson_.size() > static_cast<std::size_t>(p->location()) && p->location() >= 0);
  assert(vPerson_[p->location()].size() > static_cast<std::size_t>(p->moving_level()));
  add(p, p->location(), p->moving_level());
}

void PersonIndexByLocationMovingLevel::remove(Person *p) {
  remove_without_set_index(p);
  p->PersonIndexByLocationMovingLevelHandler::set_index(-1);
}

void PersonIndexByLocationMovingLevel::notify_change(Person *p, const Person::Property &property, const void *oldValue,
                                                     const void *newValue) {
  switch (property) {
    case Person::LOCATION:change_property(p, *(int *) newValue, p->moving_level());
      break;
    case Person::MOVING_LEVEL:change_property(p, p->location(), *(int *) newValue);
      break;
    default:break;
  }
}

std::size_t PersonIndexByLocationMovingLevel::size() const {
  return 0;
}

void PersonIndexByLocationMovingLevel::add(Person *p, const int &location, const int &moving_level) {
  auto& reference = vPerson_[location][moving_level];
  reference.push_back(p);
  p->PersonIndexByLocationMovingLevelHandler::set_index(reference.size() - 1);
}

void PersonIndexByLocationMovingLevel::remove_without_set_index(Person *p) {
  auto& reference = vPerson_[p->location()][p->moving_level()];
  reference.back()->PersonIndexByLocationMovingLevelHandler::set_index(p->PersonIndexByLocationMovingLevelHandler::index());
  reference[p->PersonIndexByLocationMovingLevelHandler::index()] = reference.back();
  reference.pop_back();
}

void PersonIndexByLocationMovingLevel::change_property(Person *p, const int &location, const int &moving_level) {
  //remove from old position
  remove_without_set_index(p); //to save 1 set and improve performance since the index of p will changed when add

  //add to new position
  add(p, location, moving_level);
}

void PersonIndexByLocationMovingLevel::defragment() {
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    for (int ml = 0; ml < Model::CONFIG->circulation_info().number_of_moving_levels; ml++) {
      std::vector<Person *>(vPerson_[location][ml]).swap(vPerson_[location][ml]);
    }
  }
}
