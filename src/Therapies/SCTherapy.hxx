/* 
 * SCTherapy.hxx
 *
 * Define and implement the single compound therapy (SCT) class.
 */

#ifndef SCTHERAPY_H
#define SCTHERAPY_H

#include "Core/PropertyMacro.h"
#include "Therapy.h"
#include <vector>

class DrugType;

class SCTherapy : public Therapy {
DISALLOW_COPY_AND_ASSIGN(SCTherapy)

public:
  int artemisinin_id;
  std::vector<int> dosing_day;

public:
  SCTherapy() : Therapy(), dosing_day{}, artemisinin_id{-1} {};
  virtual ~SCTherapy() = default;

  // Add the drug, note if it is artemisinin or not.
  void add_drug(int drug_id)  {
    Therapy::add_drug(drug_id);
    if (drug_id==0) {
      artemisinin_id = drug_id;
    }
  };

  // Return the artemisinin id or -1 if this treatment is not an artemisinin.
  int get_arteminsinin_id() const {
    return artemisinin_id;
  }

  // This is only used in MDC::record_AMU_AFU - return the maximum dosing day
  int get_max_dosing_day() const  {
    auto max = dosing_day[0];
    for (auto ndx = 1; ndx < dosing_day.size(); ndx++) {
      max = (dosing_day[ndx] > max) ? dosing_day[ndx] : max;
    }
    return max;
  }

};

#endif 