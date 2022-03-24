/* 
 * File:   TherapyBuilder.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2017, 2:43 PM
 */

#include "TherapyBuilder.h"
#include "Therapy.hxx"
#include "SCTherapy.h"
#include "MACTherapy.h"

TherapyBuilder::TherapyBuilder() = default;

TherapyBuilder::~TherapyBuilder() = default;

Therapy* TherapyBuilder::build(const YAML::Node &ns, const int &t_id) {
  Therapy* t = nullptr;
  if (ns["drug_id"]) {
    t = new SCTherapy();
    t->set_id(t_id);
    for (std::size_t i = 0; i < ns["drug_id"].size(); i++) {
      const auto drug_id = ns["drug_id"][i].as<int>();
      //        std::cout << therapy_id << "-" << drug_id << std::endl;
      dynamic_cast<SCTherapy*>(t)->add_drug(drug_id);
    }
    for (std::size_t i = 0; i < ns["dosing_days"].size(); i++) {
      const auto dosing_days = ns["dosing_days"][i].as<int>();
      dynamic_cast<SCTherapy*>(t)->dosing_day.push_back(dosing_days);
    }


  } else {
    if (ns["therapy_ids"]) {

      // Make sure that the inputs seem valid
      if (ns["therapy_ids"].size() == 0) {
        throw std::invalid_argument("At least one therapy_ids must be provided.");
      }
      if (ns["regimen"].size() == 0) {
        throw std::invalid_argument("The regimen of the therapy_ids must be provided.");
      }
      if (ns["therapy_ids"].size() != ns["regimen"].size()) {
        throw std::invalid_argument("The number of regimen entries should be the same as the number of therapy_ids");
      }

      // We are loading a more complicated protocol which consists of a therapies (i.e., drug id) 
      // given over one more more days with possible alterations to the therapy or breaks in the 
      // treatment
      t = new MACTherapy();
      t->set_id(t_id);

      for (std::size_t i = 0; i < ns["therapy_ids"].size(); i++) {
        auto therapy_id = ns["therapy_ids"][i].as<int>();
        dynamic_cast<MACTherapy*>(t)->add_therapy_id(therapy_id);
      }

      for (std::size_t i = 0; i < ns["regimen"].size(); i++) {
        auto starting_day = ns["regimen"][i].as<int>();
        dynamic_cast<MACTherapy*>(t)->add_schedule(starting_day);
      }

    }
  }

  return t;
}
