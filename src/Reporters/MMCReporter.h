
#ifndef MMCREPORTER_H
#define MMCREPORTER_H
#include "Reporter.h"
#include <sstream>

class MMCReporter : public Reporter {
DISALLOW_COPY_AND_ASSIGN(MMCReporter)
DISALLOW_MOVE(MMCReporter)

public:
  std::stringstream ss;
  const std::string group_sep = "-1111\t";
  const std::string sep = "\t";

  MMCReporter();
  virtual ~MMCReporter() = default;

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;
  void print_genotype_population_count();

  void print_genotype_frequency();


  void print_treatment_failure_rate_by_therapy();
  void monthly_report() override;

  void print_EIR_PfPR_by_location();

  void print_monthly_incidence_by_location();
};
#endif // MMCREPORTER_H
