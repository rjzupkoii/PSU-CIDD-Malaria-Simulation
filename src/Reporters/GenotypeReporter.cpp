/*
 * GenotypeReporter.cpp
 * 
 * Implementation of the GenotypeReporter class.
 */
#include "GenotypeReporter.h"

#include <cmath>
#include <string>
#include <vector>

#include "Core/Config/Config.h"
#include "Model.h"
#include "../Parasites/Genotype.h"
#include "Population/Population.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Reporter.h"

// Output genotype frequencies by the weighted number of parasite-positive individuals carrying genotype X / total number of
// parasite-positive individuals (the weights for each person describe the fraction of their clonal
// populations carrying genotype X; e.g. an individual host with five clonal infections two of which
// carry genotype X would be given a weight of 2/5).
// \param out The output stream.
void GenotypeReporter::output_genotype_weighted(std::ofstream &out) {
    
    // Check to see how many genotypes we have
    int count = Model::CONFIG->number_of_parasite_types();

    // Grab the person index
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

    // Prepare the data structures
    auto sum_all = 0.0;
    std::map<int, int> individual_genotype_map;   
    std::vector<double> results_all(count, 0.0);

    // Iterate over all of the possible locations
    for (auto loc = 0; loc < index->vPerson().size(); loc++) {
        std::vector<double> results(count, 0.0);
        auto sum = 0.0;

        // Iterate over all of the possible states
        for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            // Iterate overall of the age classes
            for (auto ac = 0; ac < index->vPerson()[0][0].size(); ac++) {
                for (auto i = 0ull; i < index->vPerson()[loc][hs][ac].size(); i++) {

                    // Get the person, press on if they are not infected (i.e., no parasites)
                    auto* person = index->vPerson()[loc][hs][ac][i];
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
            out << (sum > 0 ? (value / sum) : 0) << Tsv::sep;
        }
    }

    // Output the total for all locations
    for (auto &value : results_all) {
        out << (sum_all > 0 ? (value / sum_all) : 0) << Tsv::sep;
    }

    // Output the final total and new line
    out << sum_all << Tsv::sep;
    out << Tsv::end_line;
}

void GenotypeReporter::output_genotype_weighted_header(std::ofstream &out) {

    // Type by-line, we assume the timestamp has already been provided
    out << "Genotype frequencies by the weighted number of parasite-positive individuals carrying genotype" << Tsv::end_line;

    // Location and genotype
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        for (int type = 0; type < Model::CONFIG->number_of_parasite_types(); type++) {
            auto genotype = (*Model::CONFIG->genotype_db())[type];
            out << "Site " << location << " / " << genotype->to_string() << Tsv::sep;
        }
    }

    // Total for all locations
    for (int type = 0; type < Model::CONFIG->number_of_parasite_types(); type++) {
        auto genotype = (*Model::CONFIG->genotype_db())[type];
        out << genotype->to_string() << Tsv::sep;
    }

    // Total infected
    out << "Total Infected" << Tsv::sep;
    out << Tsv::end_line;
}