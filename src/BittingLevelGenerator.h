/* 
 * File:   BittingLevelGenerator.h
 * Author: Merlin
 *
 * Created on July 4, 2013, 5:21 PM
 */

#ifndef BITTINGLEVELGENERATOR_H
#define	BITTINGLEVELGENERATOR_H

#include "PropertyMacro.h"
#include "TypeDef.h"
class Random;

class BittingLevelGenerator {
DISALLOW_COPY_AND_ASSIGN(BittingLevelGenerator)
DISALLOW_MOVE(BittingLevelGenerator)
PROPERTY_REF(UIntVector, data)
POINTER_PROPERTY(DoubleVector, level_density)

public:
  BittingLevelGenerator();
  //    BittingLevelGenerator(const BittingLevelGenerator& orig);
  virtual ~BittingLevelGenerator();

  int draw_random_level(Random* random);

  void allocate(Random* random);

private:

};

#endif	/* BITTINGLEVELGENERATOR_H */
