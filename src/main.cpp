/* 
 * File:   main.cpp
 * Author: Merlin
 *
 * Created on August 1, 2013, 8:51 AM
 */

#include <iostream>
#include <spdlog/spdlog.h>
#include <clara.hpp>
#include "Model.h"
using namespace std;

void handle_cli(Model* model, const int argc, const char** argv);


int main(const int argc, const char* argv[]) {
  try {

    auto* m = new Model();
    handle_cli(m, argc, argv);

    m->initialize();

    m->run();

    delete m;
  }
    // Exceptions will only be thrown upon failed logger or sink construction (not during logging)
  catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Log init failed: " << ex.what() << std::endl;
    return 1;
  }
  catch (const std::exception& e) {
    std::cout << __FUNCTION__ << "-" << e.what() << std::endl;
    return 1;
  }
  return 0;
}

void handle_cli(Model* model, const int argc, const char** argv) {
  using namespace clara;
  std::string intput_filename = "input.yml";
  
  auto cli
    = Opt(intput_filename, "input_file")
    ["-i"]["--input"]
    ("The config file (YAML format). \nEx: malariasimulation -i config.yml");

  auto result = cli.parse(clara::Args(argc, argv));


  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
  }

  // Everything was ok, input_filename will have a value if supplied on command line
  model->set_config_filename(intput_filename);

}
