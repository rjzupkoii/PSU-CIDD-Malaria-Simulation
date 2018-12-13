//
// Created by Nguyen Tran on 6/25/18.
//
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#define NOMINMAX

#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>

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

  el::Configurations reporter_logger;
  reporter_logger.setToDefault();
  reporter_logger.set(el::Level::Debug, el::ConfigurationType::Format,
                      "[%level] [%logger] [%host] [%func] [%loc] %msg");
  reporter_logger.set(el::Level::Error, el::ConfigurationType::Format,
                      "[%level] [%logger] [%host] [%func] [%loc] %msg");
  reporter_logger.set(el::Level::Fatal, el::ConfigurationType::Format,
                      "[%level] [%logger] [%host] [%func] [%loc] %msg");
  reporter_logger.set(el::Level::Trace, el::ConfigurationType::Format,
                      "[%level] [%logger] [%host] [%func] [%loc] %msg");
  reporter_logger.set(el::Level::Info, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  reporter_logger.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  reporter_logger.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");

  reporter_logger.setGlobally(el::ConfigurationType::ToFile, "true");
  reporter_logger.setGlobally(el::ConfigurationType::Filename, "reporter.txt");
  reporter_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  reporter_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  // default logger uses default configurations
  el::Loggers::reconfigureLogger("reporter", reporter_logger);
  //
  // CLOG(INFO, "reporter") << "test reporter log";
  // CLOG(INFO, "reporter") << "test reporter log2";
}

int main(int argc, char *argv[]) {
  // global setup...
  config_logger();

  const auto result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}
