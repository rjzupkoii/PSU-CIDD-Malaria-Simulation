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
#include "easylogging++.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "MovementReporter.h"
#include "MMCReporter.h"
#include "SpatialReporter.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console", CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC", MMC_REPORTER},
    {"SpatialReporter", SPATIAL_REPORTER},
    {"DbReporter", DB_REPORTER},
    {"MovementReporter", MOVEMENT_REPORTER}
};

// Calculate the number of treatment failures (NTF) for the model
double Reporter::calculate_treatment_failures() {
  const auto total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) /
    static_cast<double>(Constants::DAYS_IN_YEAR());

  auto sum_ntf = 0.0;
  ul pop_size = 0;
  for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++)
  {
    sum_ntf += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
    pop_size += Model::DATA_COLLECTOR->popsize_by_location()[location];
  }

  return (sum_ntf * 100 / pop_size) / total_time_in_years;
}

Reporter *Reporter::MakeReport(ReportType report_type) {
  switch (report_type) {
    case CONSOLE:return new ConsoleReporter();
    case MONTHLY_REPORTER:return new MonthlyReporter();
    case MMC_REPORTER:return new MMCReporter();
    case SPATIAL_REPORTER: return new SpatialReporter();
    case DB_REPORTER: return new DbReporter();
    case MOVEMENT_REPORTER: return new MovementReporter();
    default:
      LOG(ERROR) << "No reporter type supplied";
      throw new std::runtime_error("No reporter type supplie");
  }
}