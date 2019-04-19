/* 
 * File:   Therapy.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 7:50 PM
 */

#ifndef SCTHERAPY_H
#define    SCTHERAPY_H

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
  SCTherapy();

  //    Therapy(const Therapy& orig);
  virtual ~SCTherapy();

  void add_drug(int drug_id);

  int get_arteminsinin_id() const;

  int get_max_dosing_day() const;
  //    int get_therapy_duration(int dosing_day);

private:

};

#endif    /* THERAPY_H */

