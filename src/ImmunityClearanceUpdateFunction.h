/* 
 * File:   ImmunityClearanceUpdateFunction.h
 * Author: Merlin
 *
 * Created on July 29, 2013, 5:49 PM
 */

#ifndef IMMUNITYCLEARANCEUPDATEFUNCTION_H
#define    IMMUNITYCLEARANCEUPDATEFUNCTION_H

#include "ParasiteDensityUpdateFunction.h"
#include "Core/PropertyMacro.h"

class Model;

class ImmunityClearanceUpdateFunction : public ParasiteDensityUpdateFunction {
 DISALLOW_COPY_AND_ASSIGN(ImmunityClearanceUpdateFunction)

 POINTER_PROPERTY(Model, model)

 public:
  explicit ImmunityClearanceUpdateFunction(Model *model = nullptr);

  //    ImmunityClearanceUpdateFunction(const ImmunityClearanceUpdateFunction& orig);
  virtual ~ImmunityClearanceUpdateFunction();

  double get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) override;

};

#endif    /* IMMUNITYCLEARANCEUPDATEFUNCTION_H */
