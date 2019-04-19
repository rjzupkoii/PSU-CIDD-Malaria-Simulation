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

std::ostream &operator<<(std::ostream &os, const Therapy &therapy) {

  os << Model::CONFIG->drug_db()->at(therapy.drug_ids[0])->name();
  for (int i = 1; i < therapy.drug_ids.size(); ++i) {
    os << "+" << Model::CONFIG->drug_db()->at(therapy.drug_ids[i])->name();
  }

  return os;
}


//int Therapy::get_therapy_duration(int dosing_day) {
//    int result = 0;
//
//    for (int i = 0; i < drug_ids_.size(); i++) {
//        DrugType* dt = Model::CONFIG->drug_db()->get(drug_ids_[i]);
//        if (!dt->is_artemisinin()) {
//            result = std::max<int>(dt->get_total_duration_of_drug_activity(dosing_day), result);
//        }
//    }
//    return result;
//}