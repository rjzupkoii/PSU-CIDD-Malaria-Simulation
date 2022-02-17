/* 
 * File:   MACTherapy.cpp
 * Author: Merlin
 * 
 * Created on November 4, 2014, 9:53 AM
 */

#include "MACTherapy.h"

#include "Core/Config/Config.h"
#include "Model.h"
#include "SCTherapy.h"

MACTherapy::MACTherapy() = default;

MACTherapy::~MACTherapy() = default;

void MACTherapy::add_therapy_id(const int &therapy_id) {
  therapy_ids_.push_back(therapy_id);
}

void MACTherapy::add_schedule(const int &start_at_day) {
  start_at_days_.push_back(start_at_day);
}

void MACTherapy::print(std::ostream& os) const {
  for (auto ndx = 0; ndx < therapy_ids_.size(); ndx++) {
    if (ndx != 0) {
      auto* previous = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db().at(therapy_ids_[ndx - 1]));
      auto separation = start_at_days_[ndx] - (start_at_days_[ndx - 1] + previous->get_max_dosing_day());
      if (separation > 0) {
        os << "/Break(" << separation << ")/";
      }
    }
    Model::CONFIG->therapy_db().at(therapy_ids_[ndx])->print(os);
  }
}