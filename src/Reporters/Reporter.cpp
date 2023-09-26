/*
 * Reporter.cpp
 *
 * Implements a factory pattern to generate the various reporter types.
 */

#include "Reporter.h"

#include "ConsoleReporter.h"
#include "Constants.h"
#include "Core/Config/Config.h"
#include "DbReporter.h"
#include "DbReporterDistrict.h"
#include "MDC/ModelDataCollector.h"
#include "MMCReporter.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "SQLiteDistrictReporter.h"
#include "Specialist/AgeBandReporter.h"
#include "Specialist/CellularReporter.h"
#include "Specialist/GenotypeCarriersReporter.h"
#include "Specialist/MovementReporter.h"
#include "Specialist/NullReporter.hxx"
#include "Specialist/PopulationReporter.h"
#include "Specialist/SeasonalImmunity.h"
#include "Specialist/TherapyRecordReporter.h"
#include "easylogging++.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console", CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC", MMC_REPORTER},
    {"DbReporter", DB_REPORTER},
    {"DbReporterDistrict", DB_REPORTER_DISTRICT},
    {"SQLiteDistrictReporter", SQLITE_DISTRICT_REPORTER},
    {"MovementReporter", MOVEMENT_REPORTER},
    {"PopulationReporter", POPULATION_REPORTER},
    {"CellularReporter", CELLULAR_REPORTER},
    {"GenotypeCarriers", GENOTYPE_CARRIERS},
    {"SeasonalImmunity", SEASONAL_IMMUNITY},
    {"AgeBand", AGE_BAND_REPORTER},
    {"TherapyRecord", THERAPY_RECORD_REPORTER},
    {"Null", NULL_REPORTER}};

Reporter *Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
  case CONSOLE:
    return new ConsoleReporter();
  case MONTHLY_REPORTER:
    return new MonthlyReporter();
  case MMC_REPORTER:
    return new MMCReporter();
  case DB_REPORTER:
    return new DbReporter();
  case DB_REPORTER_DISTRICT:
    return new DbReporterDistrict();
  case SQLITE_DISTRICT_REPORTER:
    return new SQLiteDistrictReporter();
  case MOVEMENT_REPORTER:
    return new MovementReporter();
  case POPULATION_REPORTER:
    return new PopulationReporter();
  case CELLULAR_REPORTER:
    return new CellularReporter();
  case GENOTYPE_CARRIERS:
    return new GenotypeCarriersReporter();
  case SEASONAL_IMMUNITY:
    return new SeasonalImmunity();
  case AGE_BAND_REPORTER:
    return new AgeBandReporter();
  case THERAPY_RECORD_REPORTER:
    return new TherapyRecordReporter();
  case NULL_REPORTER:
    return new NullReporter();
  default:
    LOG(ERROR) << "No reporter type supplied";
    throw std::runtime_error("No reporter type supplied");
  }
}
