/* 
 * File:   Therapy.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 7:50 PM
 */

#include "Therapy.h"
#include "Config.h"
#include "Model.h"

Therapy::Therapy() : drug_ids_(), id_(-1), number_of_dosing_days_(-1), testing_day_(-1) {
}

Therapy::~Therapy() {
}

void Therapy::add_drug(int drug_id) {
    drug_ids_.push_back(drug_id);
}

int Therapy::get_arteminsinin_id() {
    for (int i = 0; i < drug_ids_.size(); i++) {
        DrugType* dt = Model::CONFIG->drug_db()->get(drug_ids_[i]);
        if (dt->is_artemisinin()) {
            return dt->id();
        }
    }
    return -1;
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