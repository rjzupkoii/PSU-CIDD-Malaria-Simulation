/* 
 * File:   PersonIndexAll.cpp
 * Author: nguyentran
 * 
 * Created on April 17, 2013, 10:15 AM
 */

#include <vector>
#include "PersonIndexAll.h"

PersonIndexAll::PersonIndexAll() = default;

PersonIndexAll::~PersonIndexAll() {
  vPerson_.clear();

}

void PersonIndexAll::add(Person *p) {
  vPerson_.push_back(p);
  p->PersonIndexAllHandler::set_index(vPerson_.size() - 1);
}

void PersonIndexAll::remove(Person *p) {
  //move the last element to current position and remove the last holder
  vPerson_.back()->PersonIndexAllHandler::set_index(p->PersonIndexAllHandler::index());
  vPerson_[p->PersonIndexAllHandler::index()] = vPerson_.back();
  vPerson_.pop_back();
  p->PersonIndexAllHandler::set_index(-1);
  //    delete p;
  //    p = nullptr;
}

std::size_t PersonIndexAll::size() const {
  return vPerson_.size();
}

void PersonIndexAll::notify_change(Person *p, const Person::Property &property, const void *oldValue,
                                   const void *newValue) {}

void PersonIndexAll::defragment() {
  vPerson_.shrink_to_fit();
}
