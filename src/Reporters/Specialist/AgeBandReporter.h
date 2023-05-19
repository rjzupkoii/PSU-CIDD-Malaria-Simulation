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
    // Constants to track the size of the groups
    int ZERO_TO_TEN = 10;
    int FIFTEEN_TO_HUNDRED = 17;

    // When to start logging the data
    int start_recording = -1;

    // Mapping of the locations to their districts
    std::vector<int> district_lookup;

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
