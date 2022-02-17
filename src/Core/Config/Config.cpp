/* 
 * File:   Config.cpp
 * Author: nguyentran
 * 
 * Created on March 27, 2013, 10:38 AM
 */


#include <cmath>
#include <fstream>
#include <fmt/format.h>
#include "Config.h"
#include "Model.h"
#include "Core/Random.h"
#include "Therapies/SCTherapy.h"
#include "easylogging++.h"
#include "Helpers/OSHelpers.h"
#include "Helpers/StringHelpers.h"

using namespace Spatial;

Config::Config(Model* model) :
    model_(model),
    modified_mutation_factor_(-1), modified_drug_half_life_(-1),
    modified_daily_cost_of_resistance_(-1), modified_mutation_probability_(-1) {}

Config::~Config() = default;

void Config::read_from_file(const std::string &config_file_name) {
  YAML::Node config;
  try {
    config = YAML::LoadFile(config_file_name);
  }
  catch (YAML::BadFile &ex) {
    LOG(FATAL) << config_file_name << " not found or err... Ex: " << ex.msg;
    exit(EXIT_FAILURE);
  }
  catch (YAML::Exception &ex) {
    LOG(FATAL) << "error: " << ex.msg << " at line " << ex.mark.line + 1 << ":" << ex.mark.column + 1;
    exit(EXIT_FAILURE);
  }

  try {
    for (auto &config_item : config_items) {
      VLOG(1) << "Reading config item: " << config_item->name();
      config_item->set_value(config);
    }
  } catch (std::invalid_argument &error) {
    LOG(FATAL) << error.what();
    exit(EXIT_FAILURE);
  }
}
