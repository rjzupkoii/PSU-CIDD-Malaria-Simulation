//
// Created by KTT on 11/4/2021.
//

#ifndef POMS_DAILYREPORTERPRMC_H
#define POMS_DAILYREPORTERPRMC_H

#include "Reporters/Reporter.h"
#include <sstream>
#include <vector>
#include <unordered_map>

class DailyReporterPRMC {
DISALLOW_COPY_AND_ASSIGN(DailyReporterPRMC)
DISALLOW_MOVE(DailyReporterPRMC)

public:

    DailyReporterPRMC();

    ~DailyReporterPRMC();

    void initialize(int job_number, std::string path);

    void reportDaily(std::vector<std::map<std::string,int>> possible_recombined_genotype_stats);

private:
    std::stringstream ss;
};

#endif //POMS_DAILYREPORTERPRMC_H
