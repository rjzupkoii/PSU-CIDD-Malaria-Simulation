/* 
 * ImmunityClearanceUpdateFunction.h
 *
 * Define the parasite clearance due to the immune response.
 */
#ifndef IMMUNITYCLEARANCEUPDATEFUNCTION_H
#define IMMUNITYCLEARANCEUPDATEFUNCTION_H

#include "Core/PropertyMacro.h"
#include "ParasiteDensityUpdateFunction.h"

class Model;

class ImmunityClearanceUpdateFunction : public ParasiteDensityUpdateFunction {
 DISALLOW_COPY_AND_ASSIGN(ImmunityClearanceUpdateFunction)

 POINTER_PROPERTY(Model, model)

 public:
  explicit ImmunityClearanceUpdateFunction(Model *model = nullptr);

  ~ImmunityClearanceUpdateFunction() override = default;

  double get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) override;

};

#endif
