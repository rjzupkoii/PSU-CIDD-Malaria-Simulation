/* 
 * File:   ClinicalUpdateFunction.h
 * Author: Merlin
 *
 * Created on July 29, 2013, 5:43 PM
 */

#ifndef CLINICALUPDATEFUNCTION_H
#define    CLINICALUPDATEFUNCTION_H

#include "ParasiteDensityUpdateFunction.h"
#include "Core/PropertyMacro.h"

class Model;

class ClinicalUpdateFunction : public ParasiteDensityUpdateFunction {
 DISALLOW_COPY_AND_ASSIGN(ClinicalUpdateFunction)

 POINTER_PROPERTY(Model, model)

 public:
  explicit ClinicalUpdateFunction(Model *model = nullptr);
  //    ClinicalUpdateFunction(const ClinicalUpdateFunction& orig);

  virtual ~ClinicalUpdateFunction();

  double get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) override;

};

#endif    /* CLINICALUPDATEFUNCTION_H */
