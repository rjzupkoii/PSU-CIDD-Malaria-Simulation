/* 
 * File:   DrugsInBlood.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 1:47 PM
 */

#include "DrugsInBlood.h"
#include "Drug.h"
#include "Event.h"
#include "DrugType.h"
#include "Person.h"
#include "SingleHostClonalParasitePopulations.h"
#include "HelperFunction.h"

#ifndef DRUG_CUT_OFF_VALUE
#define DRUG_CUT_OFF_VALUE 0.1
#endif

OBJECTPOOL_IMPL(DrugsInBlood)

DrugsInBlood::DrugsInBlood(Person* person) : person_(person), drugs_(nullptr) {

}

void DrugsInBlood::init() {
    drugs_ = new DrugPtrMap();
}

DrugsInBlood::~DrugsInBlood() {
    if (drugs_ != nullptr) {
        clear();
        DeletePointer<DrugPtrMap>(drugs_);
    }
}

Drug* DrugsInBlood::add_drug(Drug* drug) {
    int typeID = drug->drug_type()->id();
    if (!is_drug_in_blood(typeID)) {
        drug->set_person_drugs(this);
        drugs_->insert(std::pair<int, Drug*>(typeID, drug));

        if (drug->drug_type()->is_artemisinin()) {
            person_->all_clonal_parasite_populations()->active_astermisinin_on_gametocyte(drug->drug_type());
        }
    } else {
        //already have it
        drugs_->at(typeID)->set_dosing_days(drug->dosing_days());
        drugs_->at(typeID)->set_last_update_value(drug->last_update_value());
        drugs_->at(typeID)->set_last_update_time(drug->last_update_time());
        drugs_->at(typeID)->set_start_time(drug->start_time());
        drugs_->at(typeID)->set_end_time(drug->end_time());

        //        person->cancelEvent(Events::ClearDrugFromBlood, typeID);
        //release current drug to pool
        delete drug;
    }


    return drugs_->at(typeID);

}

bool DrugsInBlood::is_drug_in_blood(DrugType* drugType) {
    return is_drug_in_blood(drugType->id());
}

bool DrugsInBlood::is_drug_in_blood(int drugTypeID) {
    return drugs_->find(drugTypeID) != drugs_->end();
}

void DrugsInBlood::remove_drug(Drug* drug) {
    remove_drug(drug->drug_type()->id());
}

void DrugsInBlood::remove_drug(const int& drugTypeID) {
    DrugPtrMap::iterator it = drugs_->find(drugTypeID);

    if (it == drugs_->end()) {
        return;
    }

    delete it->second;
    drugs_->erase(it);
}

Drug* DrugsInBlood::get_drug(const int& typeID) {
    if (!is_drug_in_blood(typeID))
        return nullptr;

    return drugs_->at(typeID);
}

int DrugsInBlood::size() {
    return drugs_->size();
}

void DrugsInBlood::clear() {
    if (drugs_->size() == 0) return;
    DrugPtrMap::iterator it;

    for (it = drugs_->begin(); it != drugs_->end(); it++) {
        delete it->second;
    }
    drugs_->clear();
}

void DrugsInBlood::update() {
    DrugPtrMap::iterator it;
    for (it = drugs_->begin(); it != drugs_->end(); it++) {
        it->second->update();
    }
}

void DrugsInBlood::clear_cut_off_drugs_by_event(Event* event) {
    if (drugs_->size() > 0) {
        DrugPtrMap::iterator pos;
        for (pos = drugs_->begin(); pos != drugs_->end();) {
            //if (pos->second->lastUpdateValue <= 0.1) {
            //Cut off at 10%
            if (pos->second->last_update_value() <= DRUG_CUT_OFF_VALUE) {
                //if drug is astermisinin then deActive Gametocyte
                if (pos->second->drug_type()->is_artemisinin()) {
                    person_->all_clonal_parasite_populations()->deactive_astermisinin_on_gametocyte();
                }

                //                person->cancelClearDrugEvent(pos->first, eventID);
                delete pos->second;
                drugs_->erase(pos++);
            } else {
                ++pos;
            }
        }
    }
}

