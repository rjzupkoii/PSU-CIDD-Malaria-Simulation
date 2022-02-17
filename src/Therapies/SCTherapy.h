/* 
 * SCTherapy.hxx
 *
 * Define and implement the single compound therapy (SCT) class.
 */
#ifndef SCTHERAPY_H
#define SCTHERAPY_H

#include "Therapy.hxx"

class DrugType;

class SCTherapy : public Therapy {
DISALLOW_COPY_AND_ASSIGN(SCTherapy)

public:
  int artemisinin_id;
  std::vector<int> dosing_day;

public:
  SCTherapy() : Therapy(), dosing_day{}, artemisinin_id{-1} {};
  ~SCTherapy() override = default;

  // Add the drug, note if it is artemisinin or not.
  void add_drug(int drug_id) override;

  // Return the artemisinin id or -1 if this treatment is not an artemisinin.
  int get_artemisinin_id() const {
    return artemisinin_id;
  }

  // This is only used in MDC::record_AMU_AFU - return the maximum dosing day
  int get_max_dosing_day() const;

  void print(std::ostream& os) const override;
};

#endif 