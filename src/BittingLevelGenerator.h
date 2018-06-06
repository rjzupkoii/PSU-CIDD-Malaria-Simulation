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
class Model;

class BittingLevelGenerator {
    DISALLOW_COPY_AND_ASSIGN_(BittingLevelGenerator)
    PROPERTY_REF(UIntVector, data)
    POINTER_PROPERTY(DoubleVector, level_density)

public:
    BittingLevelGenerator();
    //    BittingLevelGenerator(const BittingLevelGenerator& orig);
    virtual ~BittingLevelGenerator();

    int draw_random_level(Model* model);

    void allocate(Model* model);

private:

};

#endif	/* BITTINGLEVELGENERATOR_H */

