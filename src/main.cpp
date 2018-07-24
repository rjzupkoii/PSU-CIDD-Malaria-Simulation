/* 
 * File:   main.cpp
 * Author: Merlin
 *
 * Created on August 1, 2013, 8:51 AM
 */

#include <iostream>
#include <fmt/format.h>
#include "Model.h"
#include "easylogging++.h"
#include "args.hxx"
#include "Helpers/OSHelpers.h"


int job_number = 0;

INITIALIZE_EASYLOGGINGPP

using namespace std;

void handle_cli(Model* model, int argc, char** argv);

void config_logger() {
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.set(el::Level::Debug, el::ConfigurationType::Format, "[%level] [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Error, el::ConfigurationType::Format, "[%level] [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Fatal, el::ConfigurationType::Format, "[%level] [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Trace, el::ConfigurationType::Format, "[%level] [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Info, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");


  default_conf.setGlobally(el::ConfigurationType::ToFile, "false");
  default_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  default_conf.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("default", default_conf);

  el::Configurations monthly_reporter_logger;
  monthly_reporter_logger.setToDefault();
  monthly_reporter_logger.set(el::Level::Debug, el::ConfigurationType::Format,
                              "[%level] [%logger] [%host] [%func] [%loc] %msg");
  monthly_reporter_logger.set(el::Level::Error, el::ConfigurationType::Format,
                              "[%level] [%logger] [%host] [%func] [%loc] %msg");
  monthly_reporter_logger.set(el::Level::Fatal, el::ConfigurationType::Format,
                              "[%level] [%logger] [%host] [%func] [%loc] %msg");
  monthly_reporter_logger.set(el::Level::Trace, el::ConfigurationType::Format,
                              "[%level] [%logger] [%host] [%func] [%loc] %msg");
  monthly_reporter_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  monthly_reporter_logger.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  monthly_reporter_logger.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");


  monthly_reporter_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  monthly_reporter_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("monthly_data_{}.txt",job_number));
  monthly_reporter_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  monthly_reporter_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  // default logger uses default configurations
  el::Loggers::reconfigureLogger("monthly_reporter", monthly_reporter_logger);

  el::Configurations summary_reporter_logger;
  summary_reporter_logger.setToDefault();
  summary_reporter_logger.set(el::Level::Debug, el::ConfigurationType::Format,
    "[%level] [%logger] [%host] [%func] [%loc] %msg");
  summary_reporter_logger.set(el::Level::Error, el::ConfigurationType::Format,
    "[%level] [%logger] [%host] [%func] [%loc] %msg");
  summary_reporter_logger.set(el::Level::Fatal, el::ConfigurationType::Format,
    "[%level] [%logger] [%host] [%func] [%loc] %msg");
  summary_reporter_logger.set(el::Level::Trace, el::ConfigurationType::Format,
    "[%level] [%logger] [%host] [%func] [%loc] %msg");
  summary_reporter_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
  summary_reporter_logger.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  summary_reporter_logger.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");


  summary_reporter_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  summary_reporter_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("summary_{}.txt", job_number));
  summary_reporter_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  summary_reporter_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  // default logger uses default configurations
  el::Loggers::reconfigureLogger("summary_reporter", summary_reporter_logger);



  //
  // CLOG(INFO, "monthly_reporter") << "test reporter log";
  // CLOG(INFO, "monthly_reporter") << "test reporter log2";
}

int main(const int argc, char** argv) {
  try {


    auto* m = new Model();
    handle_cli(m, argc, argv);

    config_logger();
    START_EASYLOGGINGPP(argc, argv);

    m->initialize();

    m->run();

    delete m;
  }
  catch (const std::exception& e) {
    std::cout << __FUNCTION__ << "-" << e.what() << std::endl;
    return 1;
  }
  return 0;
}



void handle_cli(Model* model, int argc, char** argv) {
  args::ArgumentParser parser("Individual-based simulation for malaria.", "uut47@psu.edu");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> input_file(parser, "string",
                                          "The config file (YAML format). \nEx: malariasimulation -i input.yml",
                                            {'i', 'c', "input", "config"});

  args::ValueFlag<int> cluster_job_number(parser, "int",
                                          "Cluster job number. \nEx: malariasimulation -j 1",
                                            {'j'});
  try {
    parser.ParseCLI(argc, argv);
  }
  catch (const args::Help& e) {
    std::cout << e.what() << parser;
    exit(0);
  }
  catch (const args::ParseError& e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
  }
  catch (const args::ValidationError& e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
  }

  const auto input = input_file ? args::get(input_file) : "input.yml";

  job_number = cluster_job_number ? args::get(cluster_job_number) : 0;

  if (input != "input.yml") {
    LOG(INFO) << fmt::format("Used input file: {0}", input);
  }
  else {
    LOG(INFO) << fmt::format("Used default input file: {0}", input);
  }

  if (!OSHelpers::file_exists(input)) {
    LOG(FATAL) << fmt::format("File {0} is not exists", input);
  }
  model->set_config_filename(input);
}
