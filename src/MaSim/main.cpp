/* 
 * File:   main.cpp
 *
 * Main entry point for the simulation, reads the CLI and starts the model.
 */

#include <iostream>
#include <fmt/format.h>
#include "Model.h"
#include "easylogging++.h"
#include <args.hxx>
#include <Helpers/OSHelpers.h>
#include "Helpers/DbLoader.hxx"

// Version information
const std::string VERSION = "3.3, experimental";

// Settings read from the CLI
int job_number = 0;
std::string path = "";

INITIALIZE_EASYLOGGINGPP

void handle_cli(Model *model, int argc, char **argv);

void config_logger() {
  const std::string OUTPUT_FORMAT = "[%level] [%logger] [%host] [%func] [%loc] %msg";

  // Create the default configuration
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.set(el::Level::Debug, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Error, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Fatal, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Trace, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Info, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");
  default_conf.setGlobally(el::ConfigurationType::ToFile, "false");
  default_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  default_conf.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("default", default_conf);
}

int main(const int argc, char **argv) {

  try {

    auto *m = new Model();
    handle_cli(m, argc, argv);

    config_logger();
    START_EASYLOGGINGPP(argc, argv);
    LOG(INFO) << fmt::format("MaSim version {0}", VERSION);

    m->initialize(job_number, path);

    m->run();

    delete m;
  }
  catch (const std::exception &e) {
    std::cout << __FUNCTION__ << "-" << e.what() << std::endl;
    return 1;
  }
  return 0;
}

void handle_cli(Model *model, int argc, char **argv) {
  args::ArgumentParser parser("Individual-based simulation for malaria.", "uut47@psu.edu");

  args::Group commands(parser, "commands");
  args::HelpFlag help(commands, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> input_file(commands, "string", "The config file (YAML format). \nEx: MaSim -i input.yml", {'i', 'c', "input", "config"});
  args::ValueFlag<int> cluster_job_number(commands, "int", "Cluster job number. \nEx: MaSim -j 1", {'j'});
  args::ValueFlag<std::string> reporter(commands, "string", "Reporter Type. \nEx: MaSim -r mmc", {'r'});
  args::ValueFlag<std::string> input_path(commands, "string", "Path for output files, default is current directory. \nEx: MaSim -p out", {'o'});
  args::Flag load_genotypes(commands, "load", "Load the genotypes to the database and exit", {'l', "load"});
  
  // Allow the --v=[int] flag to be processed by START_EASYLOGGINGPP
  args::Group arguments(parser, "verbosity", args::Group::Validators::DontCare, args::Options::Global);
  args::ValueFlag<int> verbosity(arguments, "int", "Sets the current verbosity of the logging, default zero", {"v"});

  try {
    parser.ParseCLI(argc, argv);
  }
  catch (const args::Help &e) {
    std::cout << e.what() << parser;
    exit(0);
  }
  catch (const args::ParseError &e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
    exit(1);
  }
  catch (const args::ValidationError &e) {
    LOG(FATAL) << fmt::format("{0} {1}", e.what(), parser);
    exit(1);
  }

  // Check for the existence of the input file, exit if it doesn't exist.
  const auto input = input_file ? args::get(input_file) : "input.yml";
  if (!OsHelpers::file_exists(input)) {    
    LOG(FATAL) << fmt::format("File {0} is not exists. Rerun with -h or --help for help.", input);
    exit(1);
  }
  model->set_config_filename(input);
  
  // Check to see if we are doing a genotype load, do that and exit
  if (load_genotypes) {
    std::cout << "Loading genotypes...";
    DbLoader::load_genotypes(input);
    std::cout << "done!" << std::endl;
    std::cout << "Exiting simulation." << std::endl;
    exit(0);
  }

  // Set the remaining values if given
  path = input_path ? args::get(input_path) : path;
  job_number = cluster_job_number ? args::get(cluster_job_number) : 0;
  model->set_cluster_job_number(job_number);
  const auto reporter_type = reporter ? args::get(reporter) : "";
  model->set_reporter_type(reporter_type);

}
