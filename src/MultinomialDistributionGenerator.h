/* 
 * File:   MultinomialDistributionGenerator.h
 * Author: Merlin
 *
 * Created on July 4, 2013, 5:21 PM
 */

#ifndef MULTINOMIALDISTRIBUTIONGENERATOR_H
#define	MULTINOMIALDISTRIBUTIONGENERATOR_H

#include "PropertyMacro.h"
#include "TypeDef.h"
class Random;

class MultinomialDistributionGenerator {
DISALLOW_COPY_AND_ASSIGN(MultinomialDistributionGenerator)
DISALLOW_MOVE(MultinomialDistributionGenerator)
PROPERTY_REF(UIntVector, data)
POINTER_PROPERTY(DoubleVector, level_density)

public:
  MultinomialDistributionGenerator();
  //    MultinomialDistributionGenerator(const MultinomialDistributionGenerator& orig);
  virtual ~MultinomialDistributionGenerator();

  int draw_random_level(Random* random);

  void allocate(Random* random);

private:

};

#endif	/* MULTINOMIALDISTRIBUTIONGENERATOR_H */
