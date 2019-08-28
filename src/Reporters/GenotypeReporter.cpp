/*
 * GenotypeReporter.cpp
 * 
 * Implementation of the GenotypeReporter class.
 */
#include "GenotypeReporter.h"

#include <string>
#include <vector>

#include "Reporter.h"
#include "../Parasites/Genotype.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/SingleHostClonalParasitePopulations.h"

// Output genotype frequencies by the weighted number of parasite-positive individuals carrying genotype X / total number of
// parasite-positive individuals (the weights for each person describe the fraction of their clonal
// populations carrying genotype X; e.g. an individual host with five clonal infections two of which
// carry genotype X would be given a weight of 2/5).
// \param out The output stream.
// \param count Total number of genotypes defined in configuration.
// \param pi Person index by location state and ageclass that obtained from the population object.
void GenotypeReporter::output_genotype_weighted(std::ofstream &out, int count, PILSAC* pi) {
    
    auto sum_all = 0.0;
    std::map<int, int> individual_genotype_map;   
    std::vector<double> results_all(count, 0.0);

    // Iterate over all of the possible locations
    for (auto loc = 0; loc < pi->vPerson().size(); loc++) {
        std::vector<double> results(count, 0.0);
        auto sum = 0.0;

        // Iterate over all of the possible states
        for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            // Iterate overall of the age classes
            for (auto ac = 0; ac < pi->vPerson()[0][0].size(); ac++) {
                for (auto i = 0ull; i < pi->vPerson()[loc][hs][ac].size(); i++) {

                    // Get the person, press on if they are not infected (i.e., no parasites)
                    auto* person = pi->vPerson()[loc][hs][ac][i];
                    if (person->all_clonal_parasite_populations()->parasites()->empty()) {
                        continue;
                    }

                    // Update count of parasites
                    sum += 1;
                    sum_all += 1;

                    // Reset the map and start counting again for the individual
                    individual_genotype_map.clear();
                    for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
                        // Create an entry for the genotype ID, or update the count
                        if (individual_genotype_map.find(parasite_population->genotype()->genotype_id()) == individual_genotype_map.end()) {
                            individual_genotype_map[parasite_population->genotype()->genotype_id()] = 1;
                        } else {
                            individual_genotype_map[parasite_population->genotype()->genotype_id()] += 1;
                        }
                    }

                    // Copy the individual results over
                    for (const auto genotype : individual_genotype_map) {
                        results[genotype.first] += genotype.second /static_cast<double>(person->all_clonal_parasite_populations()->parasites()->size());
                        results_all[genotype.first] += genotype.second / static_cast<double>(person->all_clonal_parasite_populations()->parasites()->size());
                    }
                }
            }
        }

        // Output the total per location
        for (auto &value : results) {
            out << (value / sum) << sep;
        }
    }

    // Output the total for all locations
    for (auto &value : results_all) {
        out << (value / sum_all) << sep;
    }

    // Output the final total and new line
    out << sum_all << sep;
    out << end_line;
}

void GenotypeReporter::output_genotype_weighted_header(std::ofstream &out) {
    
}