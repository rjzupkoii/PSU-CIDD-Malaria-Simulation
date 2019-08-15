/* 
 * File:   ParasiteUpdateFunction.h
 * Author: Merlin
 *
 * Created on July 11, 2013, 2:51 PM
 */

#ifndef PARASITEDENSITYUPDATEFUNCTION_H
#define    PARASITEDENSITYUPDATEFUNCTION_H

#include "Core/PropertyMacro.h"

class ClonalParasitePopulation;

class ParasiteDensityUpdateFunction {
 DISALLOW_COPY_AND_ASSIGN(ParasiteDensityUpdateFunction)

 public:
  ParasiteDensityUpdateFunction();

  //    ParasiteUpdateFunction(const ParasiteUpdateFunction& orig);
  virtual ~ParasiteDensityUpdateFunction();

  virtual double get_current_parasite_density(ClonalParasitePopulation *parasite, int duration) = 0;

 private:

};

#endif    /* PARASITEDENSITYUPDATEFUNCTION_H */

