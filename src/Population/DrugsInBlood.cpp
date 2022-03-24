/* 
 * File:   DrugsInBlood.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 1:47 PM
 */

#include "DrugsInBlood.h"
#include "Therapies/Drug.h"
#include "Events/Event.h"
#include "Therapies/DrugType.h"
#include "Person.h"
#include "Helpers/ObjectHelpers.h"
#include "Core/TypeDef.h"

#ifndef DRUG_CUT_OFF_VALUE
#define DRUG_CUT_OFF_VALUE 0.1
#endif

OBJECTPOOL_IMPL(DrugsInBlood)

DrugsInBlood::DrugsInBlood(Person *person) : person_(person), drugs_(nullptr) {}

void DrugsInBlood::init() {
  drugs_ = new DrugPtrMap();
}

DrugsInBlood::~DrugsInBlood() {
  if (drugs_!=nullptr) {
    clear();
    ObjectHelpers::delete_pointer<DrugPtrMap>(drugs_);
  }
}

Drug *DrugsInBlood::add_drug(Drug *drug) {
  int typeID = drug->drug_type()->id();

  if (!is_drug_in_blood(typeID)) {
    drug->set_person_drugs(this);
    drugs_->insert(std::pair<int, Drug *>(typeID, drug));

  } else {
    // The drug is already present in their blood, so make sure the starting value is set correctly. This is intended
    // to occur as part fo a MACTherapy when someone is prescribed a treatment regime (ex., ACT for 3 days, 2 off, 2 on)
    drugs_->at(typeID)->set_starting_value(drug->starting_value());
    drugs_->at(typeID)->set_dosing_days(drug->dosing_days());
    drugs_->at(typeID)->set_last_update_value(drug->last_update_value());
    drugs_->at(typeID)->set_last_update_time(drug->last_update_time());
    drugs_->at(typeID)->set_start_time(drug->start_time());
    drugs_->at(typeID)->set_end_time(drug->end_time());
    delete drug;
  }

  return drugs_->at(typeID);
}

bool DrugsInBlood::is_drug_in_blood(DrugType *drug_type) const {
  return is_drug_in_blood(drug_type->id());
}

bool DrugsInBlood::is_drug_in_blood(const int drugTypeID) const {
  return drugs_->find(drugTypeID)!=drugs_->end();
}

void DrugsInBlood::remove_drug(Drug *drug) const {
  remove_drug(drug->drug_type()->id());
}

void DrugsInBlood::remove_drug(const int &drug_type_id) const {
  auto it = drugs_->find(drug_type_id);

  if (it==drugs_->end()) {
    return;
  }

  delete it->second;
  drugs_->erase(it);
}

Drug *DrugsInBlood::get_drug(const int &type_id) const {
  if (!is_drug_in_blood(type_id))
    return nullptr;

  return drugs_->at(type_id);
}

std::size_t DrugsInBlood::size() const {
  return drugs_->size();
}

void DrugsInBlood::clear() const {
  if (drugs_->empty()) return;

  for (auto &drug : *drugs_) {
    delete drug.second;
  }
  drugs_->clear();
}

void DrugsInBlood::update() const {
  for (auto &drug : *drugs_) {
    drug.second->update();
  }
}

void DrugsInBlood::clear_cut_off_drugs_by_event(Event *event) const {
  if (!drugs_->empty()) {
    for (auto pos = drugs_->begin(); pos!=drugs_->end();) {
      //if (pos->second->lastUpdateValue <= 0.1) {
      //Cut off at 10%
      if (pos->second->last_update_value() <= DRUG_CUT_OFF_VALUE) {
        //if drug is astermisinin then deActive Gametocyte

        //TODO::review
        // if (pos->second->drug_type()->is_artemisinin()) {
        //   person_->all_clonal_parasite_populations()->deactive_astermisinin_on_gametocyte();
        // }

        //                person->cancelClearDrugEvent(pos->first, eventID);
        delete pos->second;
        drugs_->erase(pos++);
      } else {
        ++pos;
      }
    }
  }
}
