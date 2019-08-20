
#ifndef MMCREPORTER_H
#define MMCREPORTER_H

#include "Reporter.h"
#include <sstream>

class MMCReporter : public Reporter {
DISALLOW_COPY_AND_ASSIGN(MMCReporter)

DISALLOW_MOVE(MMCReporter)

public:

  MMCReporter();

  ~MMCReporter() override = default;

  void initialize(int job_number, std::string path) override { }

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void print_treatment_failure_rate_by_therapy();

  void print_ntf_by_location();

  void print_genotype_frequency();

  void monthly_report() override;

  void print_EIR_PfPR_by_location();
};

#endif // MMCREPORTER_H
