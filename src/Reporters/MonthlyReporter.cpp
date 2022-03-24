//
// Created by Nguyen Tran on 3/5/2018.
//

#include "MonthlyReporter.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Random.h"
#include "Strategies/IStrategy.h"
#include "Helpers/TimeHelpers.h"
#include "Constants.h"
#include "easylogging++.h"
#include <date/date.h>
#include "Population/Population.h"
#include "ReporterUtils.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"


MonthlyReporter::MonthlyReporter() = default;

MonthlyReporter::~MonthlyReporter() = default;

void MonthlyReporter::initialize(int job_number, std::string path) {
  // Create the configuration for the monthly reporter
  el::Configurations monthly_reporter_logger;
  monthly_reporter_logger.setToDefault();
  monthly_reporter_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  monthly_reporter_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  monthly_reporter_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("{}monthly_data_{}.txt", path, job_number));
  monthly_reporter_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  monthly_reporter_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("monthly_reporter", monthly_reporter_logger);

  // Create the configuration for the summary reporter
  el::Configurations summary_reporter_logger;
  summary_reporter_logger.setToDefault();
  summary_reporter_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  summary_reporter_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  summary_reporter_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("{}summary_{}.txt", path, job_number));
  summary_reporter_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  summary_reporter_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("summary_reporter", summary_reporter_logger);
}

void MonthlyReporter::monthly_report()
{
  ss << Model::SCHEDULER->current_time() << Tsv::sep;
  ss << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << Tsv::sep;
  ss << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << Tsv::sep;
  ss << Model::CONFIG->seasonal_info()->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << Tsv::sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << Tsv::sep;
  ss << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << Tsv::sep;
  ss << Model::POPULATION->size() << Tsv::sep;
  ss << group_sep;

  print_EIR_PfPR_by_location();
  ss << group_sep;
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++)
  {
    ss << Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[loc] << Tsv::sep;
  }
  ss << group_sep;
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++)
  {
    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << Tsv::sep;
  }
  ss << group_sep;
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++)
  {
    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << Tsv::sep;
  }
  ss << group_sep;

// including total number of positive individuals
  ReporterUtils::output_genotype_frequency3(ss, Model::CONFIG->number_of_parasite_types(),
                                            Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>());


  CLOG(INFO, "monthly_reporter") << ss.str();
  ss.str("");
}

void MonthlyReporter::after_run()
{
  ss.str("");
  ss << Model::RANDOM->seed() << Tsv::sep << Model::CONFIG->number_of_locations() << Tsv::sep;
  ss << Model::CONFIG->location_db()[0].beta << Tsv::sep;
  ss << Model::CONFIG->location_db()[0].population_size << Tsv::sep;
  print_EIR_PfPR_by_location();

  ss << group_sep;
  //output last strategy information
  ss << Model::TREATMENT_STRATEGY->id << Tsv::sep;

  // output NTF
  ss << calculate_treatment_failures() << Tsv::sep;

  CLOG(INFO, "summary_reporter") << ss.str();
  ss.str("");
}

void MonthlyReporter::print_EIR_PfPR_by_location()
{
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc)
  {
    //
    // EIR
    if (Model::DATA_COLLECTOR->EIR_by_location_year()[loc].empty())
    {
      ss << 0 << Tsv::sep;
    }
    else
    {
      ss << Model::DATA_COLLECTOR->EIR_by_location_year()[loc].back() << Tsv::sep;
    }
    ss << group_sep;
    //pfpr <5 , 2-10 and all
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 2, 10) * 100 << Tsv::sep;
    ss << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 0, 5) * 100 << Tsv::sep;
    ss << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[loc] * 100 << Tsv::sep;
  }
}
