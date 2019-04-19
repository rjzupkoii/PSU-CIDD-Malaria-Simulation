/* 
 * File:   Therapy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 7:50 PM
 */

#include "SCTherapy.h"
#include "Core/Config/Config.h"
#include "Model.h"

SCTherapy::SCTherapy() : Therapy(), dosing_day{}, artemisinin_id{-1} {}

SCTherapy::~SCTherapy() = default;

void SCTherapy::add_drug(int drug_id) {
  Therapy::add_drug(drug_id);
  if (drug_id==0) {
    artemisinin_id = drug_id;
  }
}

int SCTherapy::get_arteminsinin_id() const {
  return artemisinin_id;
}

int SCTherapy::get_max_dosing_day() const {
  // TODO: implement here
  return 3;
}


//int Therapy::get_therapy_duration(int dosing_day) {
//    int result = 0;
//
//    for (int i = 0; i < drug_ids_.size(); i++) {
//        DrugType* dt = Model::CONFIG->drug_db()->get(drug_ids_[i]);
//        if (!dt->is_artemisinin()) {
//            result = std::max<int>(dt->get_duration(dosing_day), result);
//        }
//    }
//    return result;
//}
