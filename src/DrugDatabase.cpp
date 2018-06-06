/* 
 * File:   DrugDatabase.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 11:05 AM
 */

#include <boost/foreach.hpp>

#include "DrugDatabase.h"

DrugDatabase::DrugDatabase() : drug_db_() {
}

DrugDatabase::~DrugDatabase() {

    for(auto &i :  drug_db_) {
        delete i.second;
    }
    drug_db_.clear();
}

void DrugDatabase::add(DrugType* dt) {
    drug_db_[dt->id()] = dt;
}

DrugType* DrugDatabase::get(const int& id) {
    return drug_db_.at(id);
}