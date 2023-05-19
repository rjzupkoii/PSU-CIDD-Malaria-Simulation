/*
 * AgeBandReporter.h
 *
 * This reporter is intended to be used during model calibration and validation and reports the age-banded PfPR during
 * the last year of the given simulation to a CSV  file.
 */
#ifndef AGEBANDREPORTER_H
#define AGEBANDREPORTER_H

#include "Reporters/Reporter.h"

#include <vector>

class AgeBandReporter : public Reporter {
    DISALLOW_COPY_AND_ASSIGN(AgeBandReporter)
    DISALLOW_MOVE(AgeBandReporter)

private:
    // When to start logging the data
    int start_recording = -1;

    // Mapping of the locations to their districts
    std::vector<int> district_lookup;

    // String streams to use when working with the loggers
    std::stringstream pfpr;
    std::stringstream cases;

public:
    AgeBandReporter() = default;

    ~AgeBandReporter() override = default;

    void before_run() override { }

    void begin_time_step() override { }

    void initialize(int job_number, std::string path) override;

    void after_run() override { }

    void monthly_report() override;
};

#endif
