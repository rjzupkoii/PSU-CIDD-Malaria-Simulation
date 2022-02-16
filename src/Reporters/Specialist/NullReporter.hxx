/*
 * NullReporter.hxx
 *
 * This reporter is intended for situations where the model must be initialized, but will not be run as a full
 * simulation, as is the case for the DxGGenerator. Nothing is recorded, and it should not be used with anything else.
 */
#ifndef NULLREPORTER_HXX
#define NULLREPORTER_HXX

#include "Reporters/Reporter.h"

class NullReporter : public Reporter {
  public:
    NullReporter() = default;
    ~NullReporter() override = default;

    // Override everything with empty functions
    void after_run() override {}
    void before_run() override {}
    void begin_time_step() override {}
    void initialize(int job_number, std::string path) override {}
    void monthly_report() override {}
};


#endif
