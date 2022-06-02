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

PROPERTY(bool, full_compliance)

public:
  int artemisinin_id;
  std::vector<int> dosing_day;
  std::vector<double> pr_completed_days;

  SCTherapy() : Therapy(), dosing_day{}, artemisinin_id{-1}, full_compliance_{false} {};
  ~SCTherapy() override = default;

  // Add the drug, note if it is artemisinin or not.
  void add_drug(int drug_id) override;

  // Return the artemisinin id or -1 if this treatment is not an artemisinin.
  int get_artemisinin_id() const { return artemisinin_id; }

  // Return the maximum dosing day
  int get_max_dosing_day() const;

  void print(std::ostream& os) const override;
};

#endif 