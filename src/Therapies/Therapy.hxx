/* 
 * Therapy.hxx
 *
 * Defining the abstract base class for therapies.
 */

#ifndef THERAPY_HXX
#define THERAPY_HXX

#include <vector>

#include "Core/PropertyMacro.h"

class DrugType;

class Therapy {
DISALLOW_COPY_AND_ASSIGN(Therapy)

VIRTUAL_PROPERTY_REF(int, id)

VIRTUAL_PROPERTY_REF(int, testing_day)

public:
  std::vector<int> drug_ids;

public:
  Therapy() : id_{-1}, testing_day_{-1}, drug_ids{} { }

  virtual ~Therapy() = default;

  virtual void add_drug(int drug_id) {
    drug_ids.push_back(drug_id);
  }

  virtual void print(std::ostream& os) const = 0;
};

inline std::ostream& operator<<(std::ostream &os, const Therapy &therapy) {
  therapy.print(os);
  return os;
}

#endif

