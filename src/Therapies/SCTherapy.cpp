/*
 * SCTherapy.cpp
 *
 * Implement the single compound therapy class.
 */
#include "SCTherapy.h"

#include "Core/Config/Config.h"
#include "Model.h"

void SCTherapy::add_drug(int drug_id)  {
  Therapy::add_drug(drug_id);
  if (drug_id==0) {
    artemisinin_id = drug_id;
  }
}

int SCTherapy::get_max_dosing_day() const  {
  auto max = dosing_day[0];
  for (auto ndx = 1; ndx < dosing_day.size(); ndx++) {
    max = (dosing_day[ndx] > max) ? dosing_day[ndx] : max;
  }
  return max;
}

void SCTherapy::print(std::ostream& os) const {
  os << Model::CONFIG->drug_db()->at(drug_ids[0])->name();
  for (std::size_t i = 1; i < drug_ids.size(); ++i) {
    os << "+" + Model::CONFIG->drug_db()->at(drug_ids[i])->name();
  }
  os << "(" << dosing_day[0];
  for (auto ndx = 1; ndx < dosing_day.size(); ndx++) {
    os << "," << dosing_day[ndx];
  }
  os << ")";
}