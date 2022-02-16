/* 
 * File:   Therapy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 7:50 PM
 */

#include <Model.h>
#include "Therapy.h"
#include "DrugDatabase.h"
#include "Core/Config/Config.h"
#include "DrugType.h"

Therapy::Therapy() : id_{-1}, testing_day_{-1}, drug_ids{} {
}

Therapy::~Therapy() = default;

void Therapy::add_drug(int drug_id) {
  drug_ids.push_back(drug_id);
}

void Therapy::print(std::ostream& os) const {
  os << Model::CONFIG->drug_db()->at(drug_ids[0])->name();
  for (std::size_t i = 1; i < drug_ids.size(); ++i) {
    os << "+" + Model::CONFIG->drug_db()->at(drug_ids[i])->name();
  }
}

std::ostream &operator<<(std::ostream &os, const Therapy &therapy) {
  therapy.print(os);
  return os;
}