//
// Created by Nguyen Tran on 3/5/2018.
//

#ifndef POMS_BFREPORTER_H
#define POMS_BFREPORTER_H

#include "Reporter.h"
#include <sstream>

class MonthlyReporter : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(MonthlyReporter)

 DISALLOW_MOVE(MonthlyReporter)

 public:
  std::stringstream ss;
  const std::string group_sep = "-1111\t";
  const std::string sep = "\t";

  MonthlyReporter();

  ~MonthlyReporter() override;

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void monthly_report() override;

  void print_EIR_PfPR_by_location();

//  void print_monthly_incidence_by_location();
};

#endif //POMS_BFREPORTER_H
