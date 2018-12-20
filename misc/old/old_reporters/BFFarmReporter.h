//
// Created by Nguyen Tran on 3/5/2018.
//

#ifndef POMS_BFFARMREPORTER_H
#define POMS_BFFARMREPORTER_H

#include "Reporters/Reporter.h"

class BFFarmReporter : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(BFFarmReporter)

 public:
  BFFarmReporter();

  virtual ~BFFarmReporter();

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void monthly_report() override;

  void outout_parameters();
};

#endif //POMS_BFFARMREPORTER_H
