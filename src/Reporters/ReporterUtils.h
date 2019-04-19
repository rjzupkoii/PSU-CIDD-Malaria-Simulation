//
// Created by Nguyen Tran on 2019-04-18.
//

#ifndef PCMS_REPORTERUTILS_H
#define PCMS_REPORTERUTILS_H


#include <sstream>

class PersonIndexByLocationStateAgeClass;

class ReporterUtils {

public:
    /// outputs genotype frequencies by number of parasite-positive individuals carrying genotype X / total number of parasite-positive
    /// individuals
    /// \param ss the output string stream
    /// \param number_of_genotypes total number of genotypes defined in configuration
    /// \param pi person index by location state and ageclass that obtained from the population object
    static void output_genotype_frequency1(std::stringstream &ss, const int &number_of_genotypes,
                                           PersonIndexByLocationStateAgeClass* pi);

    /// outputs genotype frequencies by number of clonal parasite populations carrying genotype X / total number of clonal parasite
    ///  populations
    /// \param ss the output string stream
    /// \param number_of_genotypes total number of genotypes defined in configuration
    /// \param pi person index by location state and ageclass that obtained from the population object
    static void output_genotype_frequency2(std::stringstream &ss, const int &number_of_genotypes,
                                           PersonIndexByLocationStateAgeClass* pi);

    /// outputs genotype frequencies by the weighted number of parasite-positive individuals carrying genotype X / total number of
    /// parasite-positive individuals (the weights for each person describe the fraction of their clonal
    /// populations carrying genotype X; e.g. an individual host with five clonal infections two of which
    /// carry genotype X would be given a weight of 2/5).
    /// \param ss the output string stream
    /// \param number_of_genotypes total number of genotypes defined in configuration
    /// \param pi person index by location state and ageclass that obtained from the population object
    static void output_genotype_frequency3(std::stringstream &ss, const int &number_of_genotypes,
                                           PersonIndexByLocationStateAgeClass* pi);

    /// \brief outputs genotype frequencies by all 3 methods:
    /// \details
    ///     1. number of parasite-positive individuals carrying genotype X / total number of parasite-positive
    ///     individuals<br><br>
    ///     2. number of clonal parasite populations carrying genotype X / total number of clonal parasite
    ///     populations<br><br>
    ///     3. the weighted number of parasite-positive individuals carrying genotype X / total number of
    ///     parasite-positive individuals (the weights for each person describe the fraction of their clonal
    ///     populations carrying genotype X; e.g. an individual host with five clonal infections two of which
    ///     carry genotype X would be given a weight of 2/5).
    /// \param ss the output string stream
    /// \param number_of_genotypes total number of genotypes defined in configuration
    /// \param pi person index by location state and ageclass that obtained from the population object
    static void output_3_genotype_frequency(std::stringstream &ss, const int &number_of_genotypes,
                                            PersonIndexByLocationStateAgeClass* pi);
};


#endif //PCMS_REPORTERUTILS_H
