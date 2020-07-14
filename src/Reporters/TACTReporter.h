//
// Created by nguyentd on 7/8/2020.
//

#ifndef POMS_TACTREPORTER_H
#define POMS_TACTREPORTER_H


#include <Core/PropertyMacro.h>
#include <sstream>
#include "Reporter.h"

class PersonIndexByLocationStateAgeClass;

class TACTReporter : public Reporter {
DISALLOW_COPY_AND_ASSIGN(TACTReporter)

DISALLOW_MOVE(TACTReporter)

public:

  TACTReporter() = default;

  ~TACTReporter() override = default;

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void monthly_report() override;

private:
  void output_genotype_frequency3(
      std::stringstream& ss, const int& number_of_genotypes,
      PersonIndexByLocationStateAgeClass* pi
  );

public:
  std::stringstream ss;
  const std::string group_sep = "-1111\t";
  const std::string sep = "\t";

  long cumulative_number_of_mutation_events_last_month = 0;

};


#endif //POMS_TACTREPORTER_H
