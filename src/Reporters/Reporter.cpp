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
#include "easylogging++.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "MonthlyReporter.h"
#include "MMCReporter.h"
#include "Specialist/CellularReporter.h"
#include "Specialist/GenotypeCarriersReporter.h"
#include "Specialist/MovementReporter.h"
#include "Specialist/NullReporter.hxx"
#include "Specialist/PopulationReporter.h"
#include "Specialist/SeasonalImmunity.h"

std::map<std::string, Reporter::ReportType> Reporter::ReportTypeMap{
    {"Console", CONSOLE},
    {"MonthlyReporter", MONTHLY_REPORTER},
    {"MMC", MMC_REPORTER},
    {"DbReporter", DB_REPORTER},
    {"DbReporterDistrict", DB_REPORTER_DISTRICT},
    {"MovementReporter", MOVEMENT_REPORTER},
    {"PopulationReporter", POPULATION_REPORTER},
    {"CellularReporter", CELLULAR_REPORTER},
    {"GenotypeCarriers", GENOTYPE_CARRIERS},
    {"SeasonalImmunity", SEASONAL_IMMUNITY},
    {"Null", NULL_REPORTER}
};

// Calculate the number of treatment failures (NTF) for the model
[[deprecated("Use treatment failure count and nontreatments instead.")]]
double Reporter::calculate_treatment_failures() {
  // If the report is generated when the comparison period starts then we could end up dividing by zero, so guard against that
  const double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_of_comparison_period()) / static_cast<double>(Constants::DAYS_IN_YEAR());
  if (total_time_in_years == 0 || std::isnan(total_time_in_years)) {
    LOG(WARNING) << "Treatment failures report generated is the start of the comparison period.";
    return 0;
  }

  double sum_ntf = 0.0;
  ul pop_size = 0;
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++)
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
    case DB_REPORTER: return new DbReporter();
    case DB_REPORTER_DISTRICT: return new DbReporterDistrict();
    case MOVEMENT_REPORTER: return new MovementReporter();
    case POPULATION_REPORTER: return new PopulationReporter();
    case CELLULAR_REPORTER: return new CellularReporter();
    case GENOTYPE_CARRIERS: return new GenotypeCarriersReporter();
    case SEASONAL_IMMUNITY: return new SeasonalImmunity();
    case NULL_REPORTER: return new NullReporter();
    default:
      LOG(ERROR) << "No reporter type supplied";
      throw std::runtime_error("No reporter type supplied");
  }
}