/*
 * GenotypeReporter.h
 * 
 * Define the GenotypeReporter class which offers support for reporting genotype
 * information for the given time step.
 */
#ifndef GENOTYPEREPORTER_H
#define GENOTYPEREPORTER_H

#include <fstream>

class GenotypeReporter {
    public:
        static void output_genotype_weighted(std::ofstream &out);
};

#endif