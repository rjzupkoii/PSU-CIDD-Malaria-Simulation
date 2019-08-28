/*
 * GenotypeReporter.h
 * 
 * Define the GenotypeReporter class which offers support for reporting genotype
 * information for the given time step.
 */
#ifndef GENOTYPEREPORTER_H
#define GENOTYPEREPORTER_H

#include <fstream>

#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

// Save some typing by using a typedef
typedef PersonIndexByLocationStateAgeClass PILSAC;

class GenotypeReporter {
    protected:
        // Constants used when generating TSV files
        const std::string sep = "\t";
        const std::string end_line = "\n";

    public:
        void output_genotype_weighted(std::ofstream &out, const int count, PILSAC* pi);
        void output_genotype_weighted_header(std::ofstream &out);

};

#endif