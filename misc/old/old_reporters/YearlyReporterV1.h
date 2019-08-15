/* 
 * File:   YearlyReporterV1.h
 * Author: Merlin
 *
 * Created on January 13, 2015, 3:53 PM
 */

#ifndef YEARLYREPORTERV1_H
#define    YEARLYREPORTERV1_H

#include "Reporter.h"
#include <fstream>
#include <string>

class YearlyReporterV1 : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(YearlyReporterV1)

  std::fstream fs_;
  std::string file_name_;

 public:
  explicit YearlyReporterV1(const std::string &file_name);

  //    YearlyReporterV1(const YearlyReporterV1& orig);
  virtual ~YearlyReporterV1();

  void initialize() override;

  void before_run() override;

  void after_run() override;

  void begin_time_step() override;

  void monthly_report() override;

};

#endif    /* YEARLYREPORTERV1_H */
