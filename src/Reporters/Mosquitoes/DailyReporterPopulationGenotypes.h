//
// Created by KTT on 11/4/2021.
//

#ifndef POMS_DailyReporterPopulationGenotypes_H
#define POMS_DailyReporterPopulationGenotypes_H

#include "Reporters/Reporter.h"
#include <sstream>
#include <vector>
#include <unordered_map>

class DailyReporterPopulationGenotypes {
DISALLOW_COPY_AND_ASSIGN(DailyReporterPopulationGenotypes)
DISALLOW_MOVE(DailyReporterPopulationGenotypes)

public:

    DailyReporterPopulationGenotypes();

    ~DailyReporterPopulationGenotypes();

    void initialize(int job_number, std::string path);

    void reportDaily(std::vector<std::map<std::string,int>> population_recombined_genotype_stats);

private:
    std::stringstream ss;
};

#endif //POMS_DailyReporterPopulationGenotypes_H
