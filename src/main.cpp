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

INITIALIZE_EASYLOGGINGPP

using namespace std;

void handle_cli(Model* model, int argc, char** argv);


void config_logger() {
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.set(el::Level::Debug, el::ConfigurationType::Format, "%level [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Error, el::ConfigurationType::Format, "%level [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Fatal, el::ConfigurationType::Format, "%level [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Trace, el::ConfigurationType::Format, "%level [%logger] [%host] [%func] [%loc] %msg");
  default_conf.set(el::Level::Info, el::ConfigurationType::Format, "%level [%logger] %msg");
  default_conf.set(el::Level::Warning, el::ConfigurationType::Format, "%level [%logger] %msg");
  default_conf.set(el::Level::Verbose, el::ConfigurationType::Format, "%level-%vlevel [%logger] %msg");


  default_conf.setGlobally(el::ConfigurationType::ToFile, "false");
  default_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  default_conf.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");

  // default logger uses default configurations
  el::Loggers::reconfigureLogger("default", default_conf);
}

int main(const int argc, char** argv) {
  try {

    config_logger();

    START_EASYLOGGINGPP(argc, argv);

    auto* m = new Model();
    handle_cli(m, argc, argv);

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
  args::ArgumentParser parser("This is a test program.", "This goes after the options.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> input_file(parser, "string",
                                          "The config file (YAML format). \nEx: malariasimulation -i input.yml",
                                            {'i', 'c', "input", "config"});


  try {
    parser.ParseCLI(argc, argv);
  }
  catch (const args::Help& e) {
    std::cout << parser;
    exit(0);
  }
  catch (const args::ParseError& e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
  }
  catch (const args::ValidationError& e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
  }

  const auto input = input_file ? args::get(input_file) : "input.yml";

  if (input != "input.yml") {
    LOG(INFO) << fmt::format("Used custom input file: {0}", input);
  }
  else {
    LOG(INFO) << fmt::format("Used default input file: {0}", input);
  }
  model->set_config_filename(input);
}
