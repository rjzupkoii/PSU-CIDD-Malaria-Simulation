/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MonthlyReporter.h
 * Author: Merlin
 *
 * Created on May 10, 2016, 2:16 PM
 */

#ifndef MONTHLYREPORTER_H
#define MONTHLYREPORTER_H

#include "Reporters/Reporter.h"
#include <fstream>
#include <string>

class MonthlyReporter : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(MonthlyReporter)

 VIRTUAL_PROPERTY_REF(double, last_reported_NTF)

 VIRTUAL_PROPERTY_REF(int, last_reported_clinical_episodes)

 VIRTUAL_PROPERTY_REF(int, last_reported_mutants)

  std::fstream fs_;
  std::string file_name_;

 public:
  MonthlyReporter(const std::string &file_name);

  //    MonthlyReporter(const MonthlyReporter& orig);
  virtual ~MonthlyReporter();

  virtual void initialize();

  virtual void before_run();

  virtual void after_run();

  virtual void begin_time_step();

  virtual void monthly_report();

 private:

};

#endif /* MONTHLYREPORTER_H */

