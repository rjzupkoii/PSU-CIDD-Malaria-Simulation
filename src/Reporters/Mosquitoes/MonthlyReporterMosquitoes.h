//
// Created by kientt on 7/10/21.
//

#ifndef POMS_MONTHLYREPORTERCUSTOM_H
#define POMS_MONTHLYREPORTERCUSTOM_H

#include "Reporters/Reporter.h"
#include "DailyReporterPRMC.h"
#include <sstream>

class MonthlyReporterMosquitoes : public Reporter {
    DISALLOW_COPY_AND_ASSIGN(MonthlyReporterMosquitoes)
    DISALLOW_MOVE(MonthlyReporterMosquitoes)

public:

    MonthlyReporterMosquitoes();

    ~MonthlyReporterMosquitoes() override;

    void before_run() override;

    void begin_time_step() override { }

    void initialize(int job_number, std::string path) override;

    void after_run() override {  }

    void monthly_report() override;
};


#endif //POMS_MONTHLYREPORTERCUSTOM_H
