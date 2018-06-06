/* 
 * File:   Therapy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 7:50 PM
 */

#include "SCTherapy.h"
#include "Config.h"
#include "Model.h"

SCTherapy::SCTherapy() : drug_ids_(), dosing_day_(-1), artemisinin_id_(-1) {
}

SCTherapy::~SCTherapy() {
}

void SCTherapy::add_drug(int drug_id) {
    drug_ids_.push_back(drug_id);
    if (Model::CONFIG->drug_db()->get(drug_id)->is_artemisinin()) {
        artemisinin_id_ = drug_id;
    }
}

int SCTherapy::get_arteminsinin_id() {
   
    return artemisinin_id_;
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