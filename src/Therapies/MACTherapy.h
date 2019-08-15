/* 
 * File:   MACTherapy.h
 * Author: Merlin
 *
 * Created on November 4, 2014, 9:53 AM
 */

#ifndef MACTHERAPY_H
#define    MACTHERAPY_H

#include "Core/PropertyMacro.h"
#include <vector>
#include "Therapies/Therapy.h"

class MACTherapy : public Therapy {
 DISALLOW_COPY_AND_ASSIGN(MACTherapy)

 VIRTUAL_PROPERTY_REF(std::vector<int>, therapy_ids)

 VIRTUAL_PROPERTY_REF(std::vector<int>, start_at_days)

 public:
  MACTherapy();

  //    MACTherapy(const MACTherapy& orig);
  virtual ~MACTherapy();

  void add_therapy_id(const int &therapy_id);

  void add_schedule(const int &start_at_day);

};

#endif    /* MACTHERAPY_H */
