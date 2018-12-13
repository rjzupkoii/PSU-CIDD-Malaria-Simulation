/* 
 * File:   MultinomialDistributionGenerator.h
 * Author: Merlin
 *
 * Created on July 4, 2013, 5:21 PM
 */

#ifndef MULTINOMIALDISTRIBUTIONGENERATOR_H
#define    MULTINOMIALDISTRIBUTIONGENERATOR_H

#include "Core/TypeDef.h"

class Random;

class MultinomialDistributionGenerator {
  // DISALLOW_COPY_AND_ASSIGN(MultinomialDistributionGenerator)
  // DISALLOW_MOVE(MultinomialDistributionGenerator)


 public:
  UIntVector data;
  DoubleVector level_density;

  MultinomialDistributionGenerator() = default;

  MultinomialDistributionGenerator(const MultinomialDistributionGenerator &orig) = default;

  MultinomialDistributionGenerator &operator=(const MultinomialDistributionGenerator &) = default;

  MultinomialDistributionGenerator(MultinomialDistributionGenerator &&) = default;

  MultinomialDistributionGenerator &operator=(MultinomialDistributionGenerator &&) = default;

  virtual ~MultinomialDistributionGenerator() = default;

  int draw_random_level(Random *random);

  void allocate(Random *random);

};

#endif    /* MULTINOMIALDISTRIBUTIONGENERATOR_H */
