/*
 * PopulationReporter.h
 * 
 * Define the PopulationReporter class which is intended to dump coarse data 
 * on the number of births and deaths; as well as the total population.
 */
#ifndef POPULATIONREPORTER_H
#define POPULATIONREPORTER_H

#include "Reporters/Reporter.h"

class PopulationReporter : public Reporter {
  public:
    PopulationReporter() = default;
    ~PopulationReporter() override = default;

    // Basic declarations
    void before_run() override {}
    void after_run() override {}
    void begin_time_step() override {}

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;    
};

#endif