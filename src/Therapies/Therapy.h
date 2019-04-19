/* 
 * File:   Therapy.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 7:50 PM
 */

#ifndef THERAPY_H
#define    THERAPY_H

#include "Core/PropertyMacro.h"
#include <vector>

class DrugType;

class Therapy {
DISALLOW_COPY_AND_ASSIGN(Therapy)

VIRTUAL_PROPERTY_REF(int, id)

VIRTUAL_PROPERTY_REF(int, testing_day)

public:
  std::vector<int> drug_ids;

public:
  Therapy();

  //    Therapy(const Therapy& orig);
  virtual ~Therapy();

  virtual void add_drug(int drug_id);

  friend std::ostream &operator<<(std::ostream &os, const Therapy &therapy);

private:

};

#endif    /* THERAPY_H */

