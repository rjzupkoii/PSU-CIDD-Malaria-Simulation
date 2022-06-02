/* 
 * TherapyBuilder.cpp
 *
 * Implement the factory pattern for therapy objects.
 */
#include "TherapyBuilder.h"
#include "Therapy.hxx"
#include "SCTherapy.h"
#include "MACTherapy.h"

Therapy* TherapyBuilder::build(const YAML::Node &ns, const int &t_id) {
  if (ns["drug_id"]) {
    return create_simple(ns, t_id);
  } else if (ns["therapy_ids"]) {
    return create_complex(ns, t_id);
  } else {
    throw std::invalid_argument("The therapy does not appear to be a simple or complex type.");
  }
}

Therapy *TherapyBuilder::create_complex(const YAML::Node &ns, const int &t_id) {
  // We are parsing a complex therapy, start by making sure the inputs seem valid
  if (ns["therapy_ids"].size() == 0) {
    throw std::invalid_argument("At least one therapy_ids must be provided.");
  }
  if (ns["regimen"].size() == 0) {
    throw std::invalid_argument("The regimen of the therapy_ids must be provided.");
  }
  if (ns["therapy_ids"].size() != ns["regimen"].size()) {
    throw std::invalid_argument("The number of regimen entries should be the same as the number of therapy_ids");
  }

  // We are loading a more complicated protocol which consists of a therapies (i.e., drug id) given over one or more
  // days with possible alterations to the therapy or breaks in the treatment
  auto complex = new MACTherapy();
  complex->set_id(t_id);
  for (std::size_t i = 0; i < ns["therapy_ids"].size(); i++) {
    auto therapy_id = ns["therapy_ids"][i].as<int>();
    complex->add_therapy_id(therapy_id);
  }
  for (std::size_t i = 0; i < ns["regimen"].size(); i++) {
    auto starting_day = ns["regimen"][i].as<int>();
    complex->add_schedule(starting_day);
  }
  return complex;
}

Therapy *TherapyBuilder::create_simple(const YAML::Node &ns, const int &t_id) {
  // We are parsing a simple therapy, start by making sure the inputs seem valid
  if (ns["drug_id"].size() == 0) {
    throw std::invalid_argument("At least one drug_id must be provided.");
  }
  if (ns["dosing_days"].size() == 0) {
    throw std::invalid_argument("The number of dosing days must be must be provided.");
  }

  auto simple = new SCTherapy();
  simple->set_id(t_id);
  for (std::size_t i = 0; i < ns["drug_id"].size(); i++) {
    const auto drug_id = ns["drug_id"][i].as<int>();
    simple->add_drug(drug_id);
  }

  // TODO Determine if this can be changed from an array to a single integer value
  for (std::size_t i = 0; i < ns["dosing_days"].size(); i++) {
    const auto dosing_days = ns["dosing_days"][i].as<int>();
    simple->dosing_day.push_back(dosing_days);
  }

  if (ns["pr_completed_days"] && ns["pr_completed_days"].size() != 0) {
    // The pr_completed_days field is present, so data should be supplied for each dosing day
    if (ns["pr_completed_days"].size() != simple->get_max_dosing_day()) {
      throw std::invalid_argument("Treatment pr_completed_days should be supplied for all dosing days.");
    }

    // Read the values and track the total, so we can validate it
    auto total = 0.0;
    for (auto ndx = 0; ndx < ns["pr_completed_days"].size(); ndx++) {
      auto pr = ns["pr_completed_days"][ndx].as<double>();
      if (pr < 0 || pr > 1) {
        throw std::invalid_argument("The pr_completed_days rate per day must be between 0 and 1.");
      }
      simple->pr_completed_days.push_back(pr);
      total += pr;
    }
    if (total != 1) {
      throw std::invalid_argument("The total for pr_completed_days is " + std::to_string(total) + ", but it should equal 1.");
    }
  } else {
    // Otherwise, we assume the individual will always take the treatment each day
    simple->set_full_compliance(true);
  }

  return simple;
}
