/*
 * SpaitalReporter.h
 * 
 * Define the SpatialReporter class which generates split files when compared to the 
 * MonthlyReporter, as well as managing the increase in locations due to the maps. 
 */
#ifndef SPATIALREPORTER_H
#define SPATIALREPORTER_H

#include <ctime>
#include <fstream>
#include "Reporter.h"

class SpatialReporter : public Reporter {

 DISALLOW_COPY_AND_ASSIGN(SpatialReporter)
 DISALLOW_MOVE(SpatialReporter)

 private:
    int jobNumber;
    std::ofstream monthly;
    std::string path;
    std::time_t time;

 public:

    // Default constructor and deconstructor
    SpatialReporter() = default;
    ~SpatialReporter() override = default;

    // Basic declarations
    void before_run() override {}
    void begin_time_step() override {}

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;
    void after_run() override;

    // Report specific
    void exportEirPfPR(std::ofstream out);
    void writeMonthlyHeader();
};

#endif