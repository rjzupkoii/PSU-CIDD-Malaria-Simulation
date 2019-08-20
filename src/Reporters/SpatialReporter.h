/*
 * SpaitalReporter.h
 * 
 * Define the SpatialReporter class which generates split files when compared to the 
 * MonthlyReporter, as well as managing the increase in locations due to the maps. 
 */
#ifndef SPATIALREPORTER_H
#define SPATIALREPORTER_H

#include "Reporter.h"

class SpatialReporter : public Reporter {

 DISALLOW_COPY_AND_ASSIGN(SpatialReporter)
 DISALLOW_MOVE(SpatialReporter)

 public:

    // Default constructor and deconstructor
    SpatialReporter() = default;
    ~SpatialReporter() override = default;

    // Basic declarations
    void initialize(int job_number, std::string path) override {}
    void before_run() override {}
    void begin_time_step() override {}

    void monthly_report() override;
    
    void after_run() override;

    void exportEirPfPR();

};

#endif