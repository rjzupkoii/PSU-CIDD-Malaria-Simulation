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
#include "Strategies/IStrategy.h"
#include "Model.h"
#include "Core/Random.h"
#include "Therapies/SCTherapy.h"
#include "Strategies/StrategyBuilder.h"
#include "Therapies/TherapyBuilder.h"
#include "Strategies/NovelNonACTSwitchingStrategy.h"
#include "Strategies/NestedSwitchingStrategy.h"
#include "Strategies/NestedSwitchingDifferentDistributionByLocationStrategy.h"
#include "Helpers/NumberHelpers.h"
#include "easylogging++.h"
#include "Helpers/OSHelpers.h"
#include "Helpers/TimeHelpers.h"
#include "Helpers/StringHelpers.h"
#include "Constants.h"

using namespace Spatial;


Config::Config(Model* model) :
  model_(model), strategy_(nullptr),
  modified_mutation_factor_(-1), modified_drug_half_life_(-1),
  modified_daily_cost_of_resistance_(-1), modified_mutation_probability_(-1) {}


Config::~Config() {
  //   ObjectHelpers::DeletePointer<Strategy>(strategy_);

  for (auto& i : strategy_db_) {
    delete i;
  }

  strategy_db_.clear();
  strategy_ = nullptr;


}


void Config::read_from_file(const std::string& config_file_name) {

  YAML::Node config;
  try {
    config = YAML::LoadFile(config_file_name);
  }
  catch (YAML::BadFile& ex) {
    //FATAL??
    LOG(FATAL) << config_file_name << " not found or err...";
  }
  catch (YAML::Exception& ex) {
    //FATAL
    LOG(FATAL) << "error: " << ex.msg << " at line " << ex.mark.line + 1 << ":" << ex.mark.column + 1;
  }

  for (auto& config_item : config_items) {
    LOG(INFO) << "Reading config item: " << config_item->name();
    config_item->set_value(config);
  }

  // std::cout << typeid(*spatial_model()).name() << std::endl;
  // std::cout << "hello" << std::endl;  

  moving_level_generator_.set_level_density(&circulation_info().v_moving_level_density);
  bitting_level_generator_.set_level_density(&relative_bitting_info().v_biting_level_density);
  //
  read_strategy_therapy_and_drug_information(config);
  read_initial_parasite_info(config);
  read_importation_parasite_info(config);
  read_importation_parasite_periodically_info(config);

}


void Config::read_strategy_therapy_and_drug_information(const YAML::Node& config) {



  for (auto i = 0; i < config["StrategyInfo"].size(); i++) {
    auto* s = read_strategy(config["StrategyInfo"], i);
    strategy_db_.push_back(s);
  }

  strategy_ = strategy_db_[config["main_strategy_id"].as<int>()];


  // TODO::rework here
   if (strategy_->get_type() == IStrategy::NestedSwitching) {
     dynamic_cast<NestedSwitchingStrategy *>(strategy_)->initialize_update_time(this);
   }
  
   if (strategy_->get_type() == IStrategy::NestedSwitchingDifferentDistributionByLocation) {
     dynamic_cast<NestedSwitchingDifferentDistributionByLocationStrategy *>(strategy_)->initialize_update_time(this);
   }
}

IStrategy* Config::read_strategy(const YAML::Node& n, const int& strategy_id) {
  const auto s_id = NumberHelpers::number_to_string<int>(strategy_id);
  auto* result = StrategyBuilder::build(n[s_id], strategy_id, this);
  //    std::cout << result->to_string() << std::endl;
  return result;
}

Therapy* Config::read_therapy(const YAML::Node& n, const int& therapy_id) const {
  const auto t_id = NumberHelpers::number_to_string<int>(therapy_id);
  auto* t = TherapyBuilder::build(n[t_id], therapy_id);
  return t;
}


void Config::read_initial_parasite_info(const YAML::Node& config) {
  const auto& info_node = config["initial_parasite_info"];

  for (size_t index = 0; index < info_node.size(); index++) {
    auto location = info_node[index]["location_id"].as<int>();
    if (location < number_of_locations() && location != -1) {
      for (int j = 0; j < info_node[index]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        auto parasite_type_id = info_node[index]["parasite_info"][j]["parasite_type_id"].as<int>();
        auto prevalence = info_node[index]["parasite_info"][j]["prevalence"].as<double>();
        initial_parasite_info_.emplace_back(location, parasite_type_id, prevalence);
      }
    }
    else if (location == -1) {
      //apply for all location
      for (auto loc = 0; loc < number_of_locations(); ++loc) {
        for (auto j = 0; j < info_node[index]["parasite_info"].size(); j++) {
          //            InitialParasiteInfo ipi;
          //            ipi.location = location;
          auto parasite_type_id = info_node[index]["parasite_info"][j]["parasite_type_id"].as<int>();
          auto prevalence = info_node[index]["parasite_info"][j]["prevalence"].as<double>();
          initial_parasite_info_.emplace_back(loc, parasite_type_id, prevalence);
        }
      }
    }
  }

}

void Config::read_importation_parasite_info(const YAML::Node& config) {
  const YAML::Node& n = config["introduce_parasite"];

  for (auto i = 0; i < n.size(); i++) {
    auto location = n[i]["location"].as<int>();
    if (location < number_of_locations()) {
      for (int j = 0; j < n[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        auto parasite_type_id = n[i]["parasite_info"][j]["genotype_id"].as<int>();
        auto time = n[i]["parasite_info"][j]["time"].as<int>();
        auto num = n[i]["parasite_info"][j]["number_of_cases"].as<int>();
        importation_parasite_info_.emplace_back(location, parasite_type_id, time, num);
      }
    }
  }


}

void Config::read_importation_parasite_periodically_info(const YAML::Node& config) {
  const YAML::Node& n = config["introduce_parasite_periodically"];
  for (int i = 0; i < n.size(); i++) {
    auto location = n[i]["location"].as<int>();
    if (location < number_of_locations()) {
      for (int j = 0; j < n[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        auto parasite_type_id = n[i]["parasite_info"][j]["genotype_id"].as<int>();
        auto dur = n[i]["parasite_info"][j]["duration"].as<int>();
        auto num = n[i]["parasite_info"][j]["number_of_cases"].as<int>();
        auto start_day = n[i]["parasite_info"][j]["start_day"].as<int>();
        importation_parasite_periodically_info_.emplace_back(ImportationParasitePeriodicallyInfo{
          location, parasite_type_id, dur, num, start_day
        });
      }
    }
  }

}


void Config::override_parameters(const std::string& override_file, const int& pos) {

  if (pos == -1) return;
  std::ifstream ifs(override_file.c_str());
  if (!OSHelpers::file_exists(override_file)) {
    LOG(FATAL) << fmt::format("Override input file [{0}] is not exists", override_file);
  }

  std::string buf;
  getline(ifs, buf);

  //header of file containt all overrides parameters name
  auto override_header = StringHelpers::split(buf, '\t');


  //goto the pos line in the file
  for (int i = 0; (i < pos) && getline(ifs, buf); i++) { }
  //buff contain the parameters
  std::vector<std::string> override_parameters = StringHelpers::split(buf, '\t');

  //override parameter
  for (int i = 0; i < override_parameters.size(); i++) {
    if (override_parameters[i] != "nil") {
      override_1_parameter(override_header[i], override_parameters[i]);
    }
  }
  //re-calculate dependent variables
  //    CalculateDependentVariables();
}

void Config::override_1_parameter(const std::string& parameter_name, const std::string& parameter_value) {
  if (parameter_name == "population_size") {
    Model::CONFIG->location_db()[0].population_size = atoi(parameter_value.c_str());
  }

  if (parameter_name == "total_time") {
    total_time() = atoi(parameter_value.c_str());
  }

  if (parameter_name == "beta") {
    Model::CONFIG->location_db()[0].beta = static_cast<float>(atof(parameter_value.c_str()));
  }

  if (parameter_name == "f") {
    //        p_treatment_ = atof(parameter_value.c_str());
  }

  if (parameter_name == "daily_cost_of_resistance_factor") {
    modified_daily_cost_of_resistance_ = atof(parameter_value.c_str());

    for (auto& i : genotype_info().loci_vector) {
      for (auto& allele : i.alleles) {
        allele.daily_cost_of_resistance *= modified_daily_cost_of_resistance_;
      }
    }
    // build_parasite_db();
  }

  if (parameter_name == "daily_cost_of_resistance_copies_pfmdr") {
    double dcr = atof(parameter_value.c_str());
    // modify allele 4 5 6 7 of the mdr locus
    genotype_info().loci_vector[1].alleles[4].daily_cost_of_resistance = dcr;
    genotype_info().loci_vector[1].alleles[5].daily_cost_of_resistance =
      1 - (1 - genotype_info().loci_vector[1].alleles[1].daily_cost_of_resistance) * (1 - dcr);
    genotype_info().loci_vector[1].alleles[6].daily_cost_of_resistance =
      1 - (1 - genotype_info().loci_vector[1].alleles[2].daily_cost_of_resistance) * (1 - dcr);
    genotype_info().loci_vector[1].alleles[7].daily_cost_of_resistance =
      1 - (1 - genotype_info().loci_vector[1].alleles[3].daily_cost_of_resistance) * (1 - dcr);
    // build_parasite_db();
  }

  if (parameter_name == "z") {
    immune_system_information().immune_effect_on_progression_to_clinical = atof(parameter_value.c_str());
  }

  if (parameter_name == "kappa") {
    immune_system_information().factor_effect_age_mature_immunity = atof(parameter_value.c_str());

    // implement inlation rate
    double acR = immune_system_information().acquire_rate;
    immune_system_information().acquire_rate_by_age.clear();
    for (int age = 0; age <= 80; age++) {
      double factor = 1;
      if (age < immune_system_information().age_mature_immunity) {
        factor = (age == 0) ? 0.5 : age;

        factor = factor / immune_system_information().age_mature_immunity;
        factor = pow(factor, immune_system_information().factor_effect_age_mature_immunity);
      }

      immune_system_information().acquire_rate_by_age.push_back(factor * acR);

      acR *= (1 + immune_system_information().immune_inflation_rate);
    }

    assert(immune_system_information().acquire_rate_by_age.size() == 81);
  }

  if (parameter_name == "relative_bitting_sd") {
    relative_bitting_info().sd = atof(parameter_value.c_str());

    // TODO: rework here
    // calculate_relative_biting_density();
  }

  if (parameter_name == "k") {
    modified_mutation_factor_ = atof(parameter_value.c_str());
    for (auto& it : *drug_db()) {
      it.second->set_k(modified_mutation_factor_);
    }
  }

  if (parameter_name == "drug_half_life") {
    modified_drug_half_life_ = atof(parameter_value.c_str());
  }

  if (parameter_name == "strategy") {
    //override with the id from the override.txt
    int strategy_id = atoi(parameter_value.c_str());
    strategy_ = strategy_db_[strategy_id];
    if (strategy_->get_type() == IStrategy::NestedSwitching) {
      static_cast<NestedSwitchingStrategy *>(strategy_)->initialize_update_time(this);
    }
  }

  if (parameter_name == "dosing_days") {
    int dosing_days = atoi(parameter_value.c_str());
    for (auto& it : therapy_db()) {

      auto* scTherapy = dynamic_cast<SCTherapy *>(it);
      if (scTherapy != nullptr) {
        scTherapy->set_dosing_day(dosing_days);
      }
    }
  }

  if (parameter_name == "mutation_probability") {
    modified_mutation_probability_ = atof(parameter_value.c_str());
    for (auto& it : *drug_db()) {
      it.second->set_p_mutation(modified_mutation_probability_);

    }
    //        std::cout<< Model::CONFIG->drug_db()->drug_db().begin()->second->p_mutation() << std::endl;
  }

  if (parameter_name == "mutation_factor") {
    double mutation_factor = atof(parameter_value.c_str());
    for (auto& it : *drug_db()) {
      it.second->set_p_mutation(it.second->p_mutation() * mutation_factor);
    }
  }

  if (parameter_name == "initial_genotype") {
    int genotypeId = atoi(parameter_value.c_str());
    initial_parasite_info_[0].parasite_type_id = genotypeId;
    importation_parasite_periodically_info_[0].parasite_type_id = genotypeId;
  }

  if (parameter_name == "importation_period") {
    int importation_period = atoi(parameter_value.c_str());
    for (auto& i : importation_parasite_periodically_info_) {
      if (i.parasite_type_id == -1) {
        i.duration = importation_period;
      }
    }
  }

  if (parameter_name == "fraction_non_art_replacement") {
    const auto fnar = atof(parameter_value.c_str());
    auto* s = dynamic_cast<NovelNonACTSwitchingStrategy *>(Model::CONFIG->strategy());
    if (s != nullptr) {
      s->set_fraction_non_art_replacement(fnar);
    }
  }

  // TACT id and switching day will be modify by create new strategy in the .yml input file

}


double Config::get_seasonal_factor(const date::sys_days& today, const int& location) const {
  if (!Model::CONFIG->seasonal_info().enable) {
    return 1;
  }
  const auto day_of_year = TimeHelpers::day_of_year(today);
  const auto is_rainy_period = Model::CONFIG->seasonal_info().phi[location] < Constants::DAYS_IN_YEAR() / 2.0
                                 ? day_of_year >= Model::CONFIG->seasonal_info().phi[location]
                                 && day_of_year <= Model::CONFIG->seasonal_info().phi[location] + Constants::DAYS_IN_YEAR() / 2.0
                                 : day_of_year >= Model::CONFIG->seasonal_info().phi[location]
                                 || day_of_year <= Model::CONFIG->seasonal_info().phi[location] - Constants::DAYS_IN_YEAR() / 2.0;

  return (is_rainy_period)
           ? (Model::CONFIG->seasonal_info().A[location] - Model::CONFIG->seasonal_info().min_value[location]) *
           sin(Model::CONFIG->seasonal_info().B[location] * day_of_year + Model::CONFIG->seasonal_info().C[location]) +
           Model::CONFIG->seasonal_info().min_value[location]
           : Model::CONFIG->seasonal_info().min_value[location];
}
