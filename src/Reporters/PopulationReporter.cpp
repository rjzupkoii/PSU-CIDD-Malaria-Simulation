/*
 * PopulationReporter.cpp
 * 
 * Implement the PopulationReporter class.
 */
#include "PopulationReporter.h"

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "easylogging++.h"

void PopulationReporter::initialize(int job_number, std::string path) {
  el::Configurations reporter;
  reporter.setToDefault();
  reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  reporter.setGlobally(el::ConfigurationType::ToFile, "true");
  reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}population_data_{}.txt", path, job_number));
  reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "10");
  el::Loggers::reconfigureLogger("population_reporter", reporter);
}

void PopulationReporter::monthly_report() {
  long population = 0, births = 0, deaths = 0;

  // Calculate the summary data
  for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      population += Model::DATA_COLLECTOR->popsize_by_location()[location];
      births += Model::DATA_COLLECTOR->births_by_location()[location];
      deaths += Model::DATA_COLLECTOR->deaths_by_location()[location];
  }

  // Log the entry
  ss << Model::SCHEDULER->current_time() << Csv::sep << population 
     << Csv::sep << births << Csv::sep << deaths << Csv::sep << calculate_treatment_failures();
  CLOG(INFO, "population_reporter") << ss.str();
  ss.str("");
}