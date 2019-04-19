/* 
 * File:   DrugDatabase.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 11:05 AM
 */


#include "DrugDatabase.h"

DrugDatabase::DrugDatabase() = default;

DrugDatabase::~DrugDatabase() {

  for (auto &i : *this) {
    delete i.second;
  }
  this->clear();
}

void DrugDatabase::add(DrugType *dt) {
  (*this)[dt->id()] = dt;
}

