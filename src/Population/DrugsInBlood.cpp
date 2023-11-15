/* 
 * DrugsInBlood.cpp
 *
 * Implement the class that controls the drugs in the blood.
 */
#include "DrugsInBlood.h"

#include "Core/TypeDef.h"
#include "Helpers/ObjectHelpers.h"
#include "Person.h"
#include "Therapies/Drug.h"
#include "Therapies/DrugType.h"

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

void DrugsInBlood::clear_cut_off_drugs() const {
  // Return if the drugs are already empty
  if (drugs_->empty()) { return; }

  // Scan each of the drugs in the blood
  for (auto pos = drugs_->begin(); pos != drugs_->end();) {
    // Cut off at the defined cutoff value
    if (pos->second->last_update_value() <= DRUG_CUTOFF_VALUE) {
      delete pos->second;
      drugs_->erase(pos++);
    } else {
      ++pos;
    }
  }
}
