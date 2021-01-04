/*
 * CellularReporter.h
 * 
 * This reporter is intended to capture detailed data at the celluar level.
 */
#ifndef CELLULARREPORTER_H
#define CELLULARREPORTER_H

#include "Reporters/Reporter.h"

class CellularReporter : public Reporter {
    private:
        void detailed_report();

    public:
        CellularReporter() = default;
        ~CellularReporter() override = default;

        // Basic declarations
        void before_run() override {}
        void begin_time_step() override {}
        void after_run() override {};

        // Overrides
        void initialize(int job_number, std::string path) override;
        void monthly_report() override;
};

#endif