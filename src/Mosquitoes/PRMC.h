//
// Created by ktt5121 on 9/5/2021.
//

#ifndef POMS_PRMC_H
#define POMS_PRMC_H

#include "Core/PropertyMacro.h"
#include "Population/Properties/PersonIndexByLocationBittingLevel.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include <Parasites/Genotype.h>
#include <MDC/ModelDataCollector.h>
#include <Reporters/Mosquitoes/DailyReporterPRMC.h>
#include <Reporters/Mosquitoes/DailyReporterPopulationGenotypes.h>
#include <Core/Random.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <float.h>

class PRMC {
public:
    PRMC();
    virtual ~PRMC();
    PROPERTY_REF(int, size);
    PROPERTY_REF(date::year_month_day, starting_date);
    PROPERTY_REF(date::year_month_day, ending_date);
    PROPERTY_REF(int, starting_time);
    PROPERTY_REF(int, ending_time);
    PROPERTY_REF(bool, generate_daily_report);
    void addInterruptedFeedingRateByLocation(int location, double if_rate);
    void init();
    void initReporter(int job_number, std::string path);
    void sample(const int &current_time);
    void update(const int &current_time);
    void reset();
    Genotype* getRandomGenotype(std::size_t loc, int tracking_day);
    std::map<int,int> getTrackingGenotypeStatsByLocationAndDay(int location, int tracking_day);
    void printRecombinedGenotypeHeaders(std::stringstream& ss);
    void printRecombinedGenotypeStats(std::stringstream& ss);
    void printPopulationGenotypeHeaders(std::stringstream& ss);
    void printPopulationGenotypeStats(std::stringstream& ss);

private:
    int current_time;
    std::vector<std::vector<std::vector<Genotype*>>> genotypes_table; /* PRMC table */
    std::vector<std::vector<std::vector<Genotype*>>> genotypes_table_to_random; /* PRMC table for randomizing everyday */
    std::vector<std::map<int,int>> current_recombined_genotypes_stats; /* PRMC table statistic of current day */
    std::vector<std::vector<std::map<int,int>>> tracking_recombined_genotypes_stats; /* PRMC table statistic of all tracking days */
    std::vector<std::vector<std::map<int,int>>> tracking_recombined_genotypes_stats_to_infect; /* PRMC table statistic of all tracking days to infect in population */
    std::vector<std::vector<std::string>> possible_recombined_genotype_labels; /* all possible recombined genotype labels */
    std::vector<std::map<std::string,int>> recombined_genotype_stats; /* statistic of genotypes in PRMC table */
    std::vector<std::map<std::string,int>> population_recombined_genotype_stats; /* statistic of genotypes in population */
    DailyReporterPRMC* daily_reporter_prmc; /* reporter of PRMC genotype statistic */
    DailyReporterPopulationGenotypes* daily_reporter_population_genotypes; /* reporter of population genotype statistic */
    std::vector<double> interrupted_feeding_rate; /* current interrupted feeding rate to sample people */
    std::vector<double> interrupted_feeding_rate_from_config; /* interrupted feeding rate from config */
    std::vector<Person*> infected_persons;
    std::vector<Person*> all_persons;
    std::vector<double> br_gd_vector;
    std::vector<Genotype*> sampling_genotypes;
    std::vector<Person*> sampling_persons;
    std::vector<double> br_vector;
    void printCurrentDayInfo();
    void printTrackingDayInfo();
    void printCurrentDayStatsInfo();
    void printTrackingDayStatsInfo();
    void printTrackingDayPercentageStatsInfo();
    void addNewGenotype(int location, int position, Genotype *genotype);
    void samplingGenotypeMultinomial(int location, int position);
    void addSamplingPerson(Person* person);
    void infectNewCohortInPRMC();
    void calculatePossibleRecombinedGenotypes();
    void reportDaily();
    void updateGenotypeStats();
    void updatePopulationGenotypeStats();
    double getGenotypeRelativeInfectivity(double log_10_density);
    Genotype* recombineGenotypes(Genotype* first_genotype, Genotype* second_genotype);
    std::vector<std::map<std::string,int>> getRecombinedGenotypeStats();
    std::vector<std::map<std::string,int>> getPopulationRecombinedGenotypeStats();
    std::vector<std::string> getRecombinedGenotypeStrings(std::string genotype1, std::string genotype2);
    std::vector<std::vector<int>> getCombinationOfNumbers(std::vector<int> numbers);
};


#endif //POMS_PRMC_H
