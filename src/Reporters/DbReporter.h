#ifndef DBREPORTER_H
#define DBREPORTER_

#include "Reporter.h"

class DbReporter : public Reporter {
    public:
      DbReporter() = default;
      ~DbReporter() override = default;

      // Basic declarations
      void before_run() override {}
      void begin_time_step() override {}

      // Overrides
      void initialize(int job_number, std::string path) override;
      void monthly_report() override {}
      void after_run() override {}
};

#endif