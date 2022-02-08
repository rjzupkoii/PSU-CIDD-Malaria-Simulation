//
// Created by KTT on 11/4/2021.
//

#include "DailyReporterPRMC.h"
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
#include "Reporters/ReporterUtils.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Population/Properties/PersonIndexAll.h"
#include <math.h>
#include <filesystem>

DailyReporterPRMC::DailyReporterPRMC() = default;

DailyReporterPRMC::~DailyReporterPRMC() = default;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
inline const std::string getCurrentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M", &tstruct);

    return buf;
}

void DailyReporterPRMC::initialize(int job_number, std::string path) {
    // Create the configuration for the daily reporter
    el::Configurations daily_reporter_prmc_logger;
    daily_reporter_prmc_logger.setToDefault();
    daily_reporter_prmc_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
    daily_reporter_prmc_logger.setGlobally(el::ConfigurationType::ToFile, "true");

    ss.str("");
    std::string reporter_output_path = "Daily_PRMC_Data_";

    /* DEBUG
    // Update the study tag if one was provided
    if (!Model::MODEL->study_tag().empty()) {
        std::string study_tag = Model::MODEL->study_tag();
        LOG(INFO) << "Associating study tag " << study_tag << " with configuration.";
        ss << reporter_output_path << study_tag << "_";
        reporter_output_path = ss.str();
        ss.str("");
    }
    */

    ss << getCurrentDateTime() << "_" << reporter_output_path;
    reporter_output_path  = ss.str();
    ss.str("");
    VLOG(0) << "Report output path: " << path << reporter_output_path << job_number << ".txt";
    daily_reporter_prmc_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("{}"+ reporter_output_path +"{}.txt", path,job_number));
    daily_reporter_prmc_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    daily_reporter_prmc_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
    el::Loggers::reconfigureLogger("daily_reporter_prmc", daily_reporter_prmc_logger);

//    auto const& prmc_possible_genotype_stats = Model::CONFIG->prmc()->getRecombinedGenotypeStats();
    auto const& prmc = Model::CONFIG->prmc();
    // Log the aggregate headers
    ss
    << "Day" << Csv::sep
    << "Calendar" << Csv::sep
    ;
    prmc->printRecombinedGenotypeHeaders(ss);
//    for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
//        ss
//        << location  << ":" << "IFRate"  << Csv::sep;
//        int index = 0;
//        for(std::pair<std::string,int> stats : prmc_possible_genotype_stats[location]){
////            VLOG(0) << "[DailyReporterPRMC][initialize] size: " << prmc_possible_genotype_stats[location].size() << " stats " << stats.first << " : " << stats.second;
//            ss << location << ":" << stats.first;
//            if(index < prmc_possible_genotype_stats[location].size() - 1){
//                ss << Csv::sep;
//            }
//            index++;
//        }
//        if(location < Model::CONFIG->number_of_locations() - 1){
//            ss << Csv::sep;
//        }
//    }
    ss << Csv::end_line;
    CLOG(INFO, "daily_reporter_prmc") << ss.str();
    ss.str("");
}

void DailyReporterPRMC::reportDaily(std::vector<std::map<std::string,int>> possible_recombined_genotype_stats){
//    VLOG(0) <<"[DailyReporterPRMC][reportDaily]";
  ss
  << Model::SCHEDULER->current_time() << Csv::sep
  << date::format("%Y", Model::SCHEDULER->calendar_date) << Csv::sep;
  for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      int index = 0;
      for(auto stats : possible_recombined_genotype_stats[location]){
          ss << stats.second;
          if(index < possible_recombined_genotype_stats[location].size() - 1){
              ss << Csv::sep;
          }
          index++;
      }
      if(location < Model::CONFIG->number_of_locations() - 1){
          ss << Csv::sep;
      }
  }
  CLOG(INFO, "daily_reporter_prmc") << ss.str();
  ss.str("");
}