/* 
 * File:   Reporter.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:05 PM
 */

#include "Reporter.h"
#include "ConsoleReporter.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "MMCReporter.h"
#include "TACTReporter.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console",         CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC",             MMC_REPORTER},
    {"TACT",            TACT_REPORTER},
};

Reporter::Reporter() : model_(nullptr) {
}

Reporter::~Reporter() = default;

Reporter* Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
    case CONSOLE:return new ConsoleReporter();
    case MONTHLY_REPORTER:return new MonthlyReporter();
    case MMC_REPORTER:return new MMCReporter();
    case TACT_REPORTER: return new TACTReporter();
    default:return new MonthlyReporter();
  }
}