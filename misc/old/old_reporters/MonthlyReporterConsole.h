/* 
 * File:   MonthlyReporterConsole.h
 * Author: macpro
 *
 * Created on July 11, 2016, 1:28 PM
 */

#ifndef MONTHLYREPORTERCONSOLE_H
#define    MONTHLYREPORTERCONSOLE_H

#include "Reporters/Reporter.h"
#include <string>

class MonthlyReporterConsole : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(MonthlyReporterConsole)

 VIRTUAL_PROPERTY_REF(double, last_reported_NTF)

 VIRTUAL_PROPERTY_REF(int, last_reported_clinical_episodes)

 VIRTUAL_PROPERTY_REF(int, last_reported_mutants)

 public:
  MonthlyReporterConsole();

  //    MonthlyReporter(const MonthlyReporter& orig);
  virtual ~MonthlyReporterConsole();

  virtual void initialize();

  virtual void before_run();

  virtual void after_run();

  virtual void begin_time_step();

  virtual void monthly_report();

 private:

};

#endif    /* MONTHLYREPORTERCONSOLE_H */

