/* 
 * File:   Config.cpp
 * Author: nguyentran
 * 
 * Created on March 27, 2013, 10:38 AM
 */


#include <gsl/gsl_cdf.h>
#include <cmath>
#include <fstream>
#include <fmt/format.h>
#include "Config.h"
#include "Strategies/IStrategy.h"
#include "Model.h"
#include "Core/Random.h"
#include "SCTherapy.h"
#include "Strategies/StrategyBuilder.h"
#include "TherapyBuilder.h"
#include "Strategies/NovelNonACTSwitchingStrategy.h"
#include "Strategies/NestedSwitchingStrategy.h"
#include "Spatial/SpatialModelBuilder.h"
#include "Strategies/NestedSwitchingDifferentDistributionByLocationStrategy.h"
#include "Helpers/NumberHelpers.h"
#include "easylogging++.h"
#include "Helpers/OSHelpers.h"
#include "Helpers/TimeHelpers.h"
#include "Helpers/StringHelpers.h"
#include "Helpers/ObjectHelpers.h"
#include "Constants.h"

using namespace Spatial;


Config::Config(Model* model) :
  model_(model), number_of_parasite_types_(-1), log_parasite_density_level_(),
  relative_bitting_information_(),
  relative_infectivity_(), strategy_(nullptr),
  drug_db_(nullptr), genotype_db_(nullptr),
  days_to_clinical_under_five_(-1), days_to_clinical_over_five_(-1), days_mature_gametocyte_under_five_(-1),
  days_mature_gametocyte_over_five_(-1), p_compliance_{-1}, min_dosing_days_(-1),
  gametocyte_level_under_artemisinin_action_(-1), gametocyte_level_full_(-1), p_relapse_(-1),
  relapse_duration_(-1), allow_new_coinfection_to_cause_symtoms_(false), update_frequency_(-1), report_frequency_(-1),
  circulation_information_(), TF_rate_(-1), tme_info_(), tme_strategy_(nullptr),
  modified_mutation_factor_(-1), modified_drug_half_life_(-1), using_free_recombination_(false), tf_testing_day_(-1),
  tf_window_size_(-1), using_age_dependent_bitting_level_(false),
  using_variable_probability_infectious_bites_cause_infection_(false), fraction_mosquitoes_interrupted_feeding_(0),
  modified_daily_cost_of_resistance_(-1), modified_mutation_probability_(-1),
  spatial_model_(nullptr),
  inflation_factor_{1} {}

Config::~Config() {
  //   ObjectHelpers::DeletePointer<Strategy>(strategy_);
  ObjectHelpers::delete_pointer<IStrategy>(tme_strategy_);
  ObjectHelpers::delete_pointer<DrugDatabase>(drug_db_);
  ObjectHelpers::delete_pointer<IntGenotypeDatabase>(genotype_db_);

  for (auto& i : strategy_db_) {
    delete i;
  }

  strategy_db_.clear();

  strategy_ = nullptr;

  for (auto& i : therapy_db_) {
    delete i;
  }
  therapy_db_.clear();

  ObjectHelpers::delete_pointer<SpatialModel>(spatial_model_);
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

  // std::cout << number_of_locations() << std::endl;

  //read spatial_model
  const std::string sm_name = config["spatial_model"]["name"].as<std::string>();
  spatial_model_ = SpatialModelBuilder::Build(sm_name, config["spatial_model"][sm_name]);
  // std::cout << "hello" << std::endl;

  read_biodemography_information(config);

  read_parasite_density_level(config["parasite_density_level"]);

  read_immune_system_information(config["immune_system_information"]);

  read_strategy_therapy_and_drug_information(config);

  read_relative_biting_rate_info(config);
  read_spatial_info(config);
  
  read_initial_parasite_info(config);
  read_importation_parasite_info(config);
  read_importation_parasite_periodically_info(config);

  read_relative_infectivity_info(config);

  days_to_clinical_under_five_ = config["days_to_clinical_under_five"].as<int>();
  days_to_clinical_over_five_ = config["days_to_clinical_over_five"].as<int>();
  days_mature_gametocyte_under_five_ = config["days_mature_gametocyte_under_five"].as<int>();
  days_mature_gametocyte_over_five_ = config["days_mature_gametocyte_over_five"].as<int>();

  p_compliance_ = config["p_compliance"].as<double>();
  min_dosing_days_ = config["min_dosing_days"].as<int>();

  gametocyte_level_under_artemisinin_action_ = config["gametocyte_level_under_artemisinin_action"].as<double>();
  gametocyte_level_full_ = config["gametocyte_level_full"].as<double>();

  p_relapse_ = config["p_relapse"].as<double>();
  relapse_duration_ = config["relapse_duration"].as<int>();

  allow_new_coinfection_to_cause_symtoms_ = config["allow_new_coinfection_to_cause_symtoms"].as<bool>();
  update_frequency_ = config["update_frequency"].as<int>();
  report_frequency_ = config["report_frequency"].as<int>();

  TF_rate_ = config["TF_rate"].as<double>();

  tme_info_.tme_starting_day = config["tme_info"]["tme_starting_day"].as<int>();
  tme_info_.MDA_coverage.clear();
  tme_info_.MDA_duration.clear();

  if (config["tme_info"]["mda_coverage"].size() < number_of_locations()) {
    for (int location = 0; location < number_of_locations(); location++) {
      tme_info_.MDA_coverage.push_back(config["tme_info"]["mda_coverage"][0].as<double>());
      tme_info_.MDA_duration.push_back(config["tme_info"]["mda_duration"][0].as<int>());
    }
  }
  else {
    for (int location = 0; location < number_of_locations(); location++) {
      tme_info_.MDA_coverage.push_back(config["tme_info"]["mda_coverage"][location].as<double>());
      tme_info_.MDA_duration.push_back(config["tme_info"]["mda_duration"][location].as<int>());
    }
  }
  using_free_recombination_ = config["using_free_recombination"].as<bool>();
  tf_window_size_ = config["tf_window_size"].as<int>();

  using_age_dependent_bitting_level_ = config["using_age_dependent_bitting_level"].as<bool>();
  using_variable_probability_infectious_bites_cause_infection_ = config[
    "using_variable_probability_infectious_bites_cause_infection"].as<bool>();
  fraction_mosquitoes_interrupted_feeding_ = config["fraction_mosquitoes_interrupted_feeding"].as<double>();
  inflation_factor_ = config["inflation_factor"].as<double>();

}

void Config::read_parasite_density_level(const YAML::Node& config) {

  log_parasite_density_level_.log_parasite_density_cured = config["log_parasite_density_cured"].as<double>();
  log_parasite_density_level_.log_parasite_density_from_liver = config["log_parasite_density_from_liver"].as<double>();
  log_parasite_density_level_.log_parasite_density_asymptomatic = config["log_parasite_density_asymptomatic"].as<double
  >();
  log_parasite_density_level_.log_parasite_density_clinical = config["log_parasite_density_clinical"].as<double>();
  log_parasite_density_level_.log_parasite_density_clinical_from = config["log_parasite_density_clinical_from"].as<
    double>();
  log_parasite_density_level_.log_parasite_density_clinical_to = config["log_parasite_density_clinical_to"].as<double
  >();
  log_parasite_density_level_.log_parasite_density_detectable = config["log_parasite_density_detectable"].as<double>();
  log_parasite_density_level_.log_parasite_density_pyrogenic = config["log_parasite_density_pyrogenic"].as<double>();
}

void Config::read_immune_system_information(const YAML::Node& config) {

  immune_system_information_.acquire_rate = config["b1"].as<double>();
  immune_system_information_.decay_rate = config["b2"].as<double>();

  immune_system_information_.duration_for_fully_immune = config["duration_for_fully_immune"].as<double>();
  immune_system_information_.duration_for_naive = config["duration_for_naive"].as<double>();

  const auto mean_initial_condition = config["mean_initial_condition"].as<double>();
  const auto sd_initial_condition = config["sd_initial_condition"].as<double>();

  if (NumberHelpers::is_equal(sd_initial_condition, 0.0)) {
    immune_system_information_.alpha_immune = mean_initial_condition;
    immune_system_information_.beta_immune = 0.0;
  }
  else {
    immune_system_information_.alpha_immune =
      mean_initial_condition * mean_initial_condition * (1 - mean_initial_condition) /
      (sd_initial_condition * sd_initial_condition) - mean_initial_condition;
    immune_system_information_.beta_immune = immune_system_information_.alpha_immune / mean_initial_condition -
      immune_system_information_.alpha_immune;
  }

  immune_system_information_.immune_inflation_rate = config["immune_inflation_rate"].as<double>();

  immune_system_information_.min_clinical_probability = config["min_clinical_probability"].as<double>();
  immune_system_information_.max_clinical_probability = config["max_clinical_probability"].as<double>();

  immune_system_information_.immune_effect_on_progression_to_clinical = config[
    "immune_effect_on_progression_to_clinical"].as<double>();

  immune_system_information_.c_min = pow(10, -(log_parasite_density_level_.log_parasite_density_asymptomatic -
                                           log_parasite_density_level_.log_parasite_density_cured) /
                                         immune_system_information_.duration_for_fully_immune);
  immune_system_information_.c_max = pow(10, -(log_parasite_density_level_.log_parasite_density_asymptomatic -
                                           log_parasite_density_level_.log_parasite_density_cured) /
                                         immune_system_information_.duration_for_naive);
  //    std::cout << immune_system_information_.c_min << std::endl;
  //    std::cout << immune_system_information_.c_max << std::endl;


  immune_system_information_.age_mature_immunity = config["age_mature_immunity"].as<double>();
  immune_system_information_.factor_effect_age_mature_immunity = config["factor_effect_age_mature_immunity"].as<double
  >();

  // implement inlation rate
  double acR = immune_system_information_.acquire_rate;
  immune_system_information_.acquire_rate_by_age.clear();
  for (int age = 0; age <= 80; age++) {
    double factor = 1;
    if (age < immune_system_information().age_mature_immunity) {
      factor = (age == 0) ? 0.5 : age;

      factor = factor / immune_system_information().age_mature_immunity;
      factor = pow(factor, immune_system_information().factor_effect_age_mature_immunity);
    }

    immune_system_information_.acquire_rate_by_age.push_back(factor * acR);

    acR *= (1 + immune_system_information_.immune_inflation_rate);
    //        std::cout << acR << std::endl;
  }
  assert(immune_system_information_.acquire_rate_by_age.size() == 81);

}

void Config::read_strategy_therapy_and_drug_information(const YAML::Node& config) {
  read_genotype_info(config);
  build_drug_and_parasite_db(config);

  // read tf_testing_day
  tf_testing_day_ = config["tf_testing_day"].as<int>();

  //    read_all_therapy
  for (int i = 0; i < config["TherapyInfo"].size(); i++) {
    auto* t = read_therapy(config["TherapyInfo"], i);
    therapy_db_.push_back(t);
  }

  for (auto i = 0; i < config["StrategyInfo"].size(); i++) {
    auto* s = read_strategy(config["StrategyInfo"], i);
    strategy_db_.push_back(s);
  }

  strategy_ = strategy_db_[config["main_strategy_id"].as<int>()];

  if (strategy_->get_type() == IStrategy::NestedSwitching) {
    static_cast<NestedSwitchingStrategy *>(strategy_)->initialize_update_time();
  }

  if (strategy_->get_type() == IStrategy::NestedSwitchingDifferentDistributionByLocation) {
    static_cast<NestedSwitchingDifferentDistributionByLocationStrategy *>(strategy_)->initialize_update_time();
  }
}

IStrategy* Config::read_strategy(const YAML::Node& n, const int& strategy_id) const {
  const auto s_id = NumberHelpers::number_to_string<int>(strategy_id);

  auto* result = StrategyBuilder::build(n[s_id], strategy_id);

  //    std::cout << result->to_string() << std::endl;
  return result;
}

Therapy* Config::read_therapy(const YAML::Node& n, const int& therapy_id) const {
  const auto t_id = NumberHelpers::number_to_string<int>(therapy_id);
  auto* t = TherapyBuilder::build(n[t_id], therapy_id);

  return t;
}

void Config::build_drug_and_parasite_db(const YAML::Node& config) {
  // build parasite db
  build_parasite_db();

  //build drug DB
  build_drug_db(config);

}

void Config::read_genotype_info(const YAML::Node& config) {
  genotype_info_.loci_vector.clear();
  for (int i = 0; i < config["genotype_info"]["loci"].size(); i++) {
    Locus l;
    l.position = config["genotype_info"]["loci"][i]["position"].as<int>();


    for (int j = 0; j < config["genotype_info"]["loci"][i]["alleles"].size(); j++) {
      Allele al;
      al.value = config["genotype_info"]["loci"][i]["alleles"][j]["value"].as<int>();
      al.name = config["genotype_info"]["loci"][i]["alleles"][j]["allele_name"].as<std::string>();
      al.short_name = config["genotype_info"]["loci"][i]["alleles"][j]["short_name"].as<std::string>();
      al.mutation_level = config["genotype_info"]["loci"][i]["alleles"][j]["mutation_level"].as<int>();
      al.daily_cost_of_resistance = config["genotype_info"]["loci"][i]["alleles"][j]["daily_cost_of_resistance"].as<
        double>();
      for (int c = 0; c < config["genotype_info"]["loci"][i]["alleles"][j]["can_mutate_to"].size(); c++) {
        //                al.mutation_value_up.push_back(config["genotype_info"]["loci"][i]["alleles"][j]["mutation_up"][c].as<int>());
        al.mutation_values.push_back(
          config["genotype_info"]["loci"][i]["alleles"][j]["can_mutate_to"][c].as<int>());
      }

      l.alleles.push_back(al);
    }

    genotype_info_.loci_vector.push_back(l);
  }
  //
  //    for (int c = 0; c < genotype_info_.loci_vector[2].alleles[2].mutation_values.size(); c++) {
  //        std::cout << genotype_info_.loci_vector[2].alleles[2].mutation_values[c] << std::endl;
  //
  //    }
}

void Config::build_drug_db(const YAML::Node& config) {
  ObjectHelpers::delete_pointer<DrugDatabase>(drug_db_);
  drug_db_ = new DrugDatabase();

  for (int i = 0; i < config["drugInfo"].size(); i++) {
    DrugType* dt = read_drugtype(config, i);
    //        std::cout << i << std::endl;
    drug_db_->add(dt);

  }


  //get EC50 table and compute EC50^n
  EC50_power_n_table_.clear();
  EC50_power_n_table_.assign(genotype_db_->db().size(), std::vector<double>());

  for (int g_id = 0; g_id < genotype_db_->db().size(); g_id++) {
    for (int i = 0; i < drug_db_->drug_db().size(); i++) {
      EC50_power_n_table_[g_id].push_back(drug_db_->drug_db()[i]->infer_ec50(genotype_db_->db()[g_id]));
    }
  }
  //    std::cout << "ok " << std::endl;

  for (int g_id = 0; g_id < genotype_db_->db().size(); g_id++) {
    for (int i = 0; i < drug_db_->drug_db().size(); i++) {
      EC50_power_n_table_[g_id][i] = pow(EC50_power_n_table_[g_id][i], drug_db_->get(i)->n());
    }
  }
}

void Config::build_parasite_db() {

  ObjectHelpers::delete_pointer<IntGenotypeDatabase>(genotype_db_);
  genotype_db_ = new IntGenotypeDatabase();

  int number_of_genotypes = 1;
  for (auto& i : genotype_info_.loci_vector) {
    number_of_genotypes *= i.alleles.size();
  }

  for (int i = 0; i < number_of_genotypes; i++) {
    auto* int_genotype = new IntGenotype(i);
    //        std::cout << *int_genotype << std::endl;
    genotype_db_->add(int_genotype);
  }

  genotype_db_->initialize_matting_matrix();
  number_of_parasite_types_ = static_cast<int>(genotype_db_->db().size());
}

DrugType* Config::read_drugtype(const YAML::Node& config, const int& drug_id) const {
  auto* dt = new DrugType();
  dt->set_id(drug_id);

  const auto drug_id_s = NumberHelpers::number_to_string<int>(drug_id);
  const auto& n = config["drugInfo"][drug_id_s];

  dt->set_drug_half_life(n["half_life"].as<double>());
  dt->set_maximum_parasite_killing_rate(n["maximum_parasite_killing_rate"].as<double>());
  dt->set_n(n["n"].as<double>());
  //    dt->set_EC50(n["EC50"].as<double>());

  //    std::cout <<dt->drug_half_life() << "-" << dt->maximum_parasite_killing_rate() << "-" << dt->n() << "-" << dt->EC50() << std::endl;
  for (int i = 0; i < n["age_specific_drug_concentration_sd"].size(); i++) {
    dt->age_group_specific_drug_concentration_sd().push_back(
      n["age_specific_drug_concentration_sd"][i].as<double>());
  }
  //    assert(dt->age_group_specific_drug_concentration_sd().size() == 15);

  dt->set_p_mutation(n["mutation_probability"].as<double>());

  dt->affecting_loci().clear();
  for (int i = 0; i < n["affecting_loci"].size(); i++) {
    dt->affecting_loci().push_back(n["affecting_loci"][i].as<int>());
  }

  dt->selecting_alleles().clear();
  dt->selecting_alleles().assign(n["affecting_loci"].size(), IntVector());
  for (int i = 0; i < n["affecting_loci"].size(); i++) {
    for (int j = 0; j < n["selecting_alleles"][i].size(); j++) {
      dt->selecting_alleles()[i].push_back(n["selecting_alleles"][i][j].as<int>());

    }
  }

  dt->set_ec50_map(n["EC50"].as<std::map<std::string, double>>());

  //    auto ec50Node = n["EC50"];
  //    for (YAML::const_iterator it = ec50Node.begin(); it != ec50Node.end(); it++) {
  //        std::string key = it->first.as<std::string>();
  //        double value = it->second.as<double>();
  //        std::cout << key << ":::::" << value << std::endl;
  //    }

  dt->set_k(n["k"].as<double>());


  if (drug_id == config["artemisinin_drug_id"].as<int>()) {
    dt->set_drug_family(DrugType::Artemisinin);
  }
  else if (drug_id == config["lumefantrine_drug_id"].as<int>()) {
    dt->set_drug_family(DrugType::Lumefantrine);
  }
  else {
    dt->set_drug_family(DrugType::Other);
  }
  return dt;
}

void Config::read_relative_biting_rate_info(const YAML::Node& config) {
  const YAML::Node& n = config["relative_bitting_info"];

  relative_bitting_information_.max_relative_biting_value = n["max_relative_biting_value"].as<double>();

  relative_bitting_information_.number_of_biting_levels = n["number_of_biting_levels"].as<int>();

  relative_bitting_information_.scale = n["biting_level_distribution"]["Exponential"]["scale"].as<double>();

  relative_bitting_information_.mean = n["biting_level_distribution"]["Gamma"]["mean"].as<double>();
  relative_bitting_information_.sd = n["biting_level_distribution"]["Gamma"]["sd"].as<double>();

  //calculate density and level value here
  calculate_relative_biting_density();
}

void Config::calculate_relative_biting_density() {
  double var = relative_bitting_information_.sd * relative_bitting_information_.sd;
  double b = var / (relative_bitting_information_.mean - 1); //theta
  double a = (relative_bitting_information_.mean - 1) / b; //k

  relative_bitting_information_.v_biting_level_density.clear();
  relative_bitting_information_.v_biting_level_value.clear();

  double max = relative_bitting_information_.max_relative_biting_value - 1; //maxRelativeBiting -1
  int numberOfLevel = relative_bitting_information_.number_of_biting_levels;

  double step = max / (double)(numberOfLevel - 1);

  int j = 0;
  double oldP = 0;
  double sum = 0;

  for (double i = 0; i <= max + 0.0001; i += step) {
    double p = gsl_cdf_gamma_P(i + step, a, b);
    double value = 0;
    value = (j == 0) ? p : p - oldP;
    relative_bitting_information_.v_biting_level_density.push_back(value);
    oldP = p;
    relative_bitting_information_.v_biting_level_value.push_back(i + 1);
    sum += value;
    j++;

  }


  //normalized
  double t = 0;
  for (double& i : relative_bitting_information_.v_biting_level_density) {
    i = i + (1 - sum) / relative_bitting_information_.v_biting_level_density.size();
    t += i;
  }


  assert(relative_bitting_information_.number_of_biting_levels ==
    relative_bitting_information_.v_biting_level_density.size());
  assert(relative_bitting_information_.number_of_biting_levels ==
    relative_bitting_information_.v_biting_level_value.size());
  assert(fabs(t - 1) < 0.0001);
  bitting_level_generator_.set_level_density(&relative_bitting_information_.v_biting_level_density);


  //    for (int i = 0; i < relative_bitting_information_.v_biting_level_density.size(); i++) {
  //        std::cout << i << "\t" << relative_bitting_information_.v_biting_level_value[i] << "\t"
  //                << relative_bitting_information_.v_biting_level_density[i] << std::endl;
  //    }


}

void Config::read_spatial_info(const YAML::Node& config) {
  const YAML::Node& n = config["circulation_information"];

  circulation_information_.max_relative_moving_value = n["max_relative_moving_value"].as<double>();

  circulation_information_.number_of_moving_levels = n["number_of_moving_levels"].as<int>();

  circulation_information_.scale = n["moving_level_distribution"]["Exponential"]["scale"].as<double>();

  circulation_information_.mean = n["moving_level_distribution"]["Gamma"]["mean"].as<double>();
  circulation_information_.sd = n["moving_level_distribution"]["Gamma"]["sd"].as<double>();

  //calculate density and level value here

  double var = circulation_information_.sd * circulation_information_.sd;

  double b = var / (circulation_information_.mean - 1); //theta
  double a = (circulation_information_.mean - 1) / b; //k

  circulation_information_.v_moving_level_density.clear();
  circulation_information_.v_moving_level_value.clear();

  const auto max = circulation_information_.max_relative_moving_value - 1; //maxRelativeBiting -1
  const auto number_of_level = circulation_information_.number_of_moving_levels;

  const auto step = max / static_cast<double>(number_of_level - 1);

  auto j = 0;
  double old_p = 0;
  double sum = 0;

  for (double i = 0; i <= max + 0.0001; i += step) {
    const auto p = gsl_cdf_gamma_P(i + step, a, b);
    double value = 0;
    value = (j == 0) ? p : p - old_p;
    circulation_information_.v_moving_level_density.push_back(value);
    old_p = p;
    circulation_information_.v_moving_level_value.push_back(i + 1);
    sum += value;
    j++;

  }

  //normalized
  double t = 0;
  for (auto& i : circulation_information_.v_moving_level_density) {
    i = i + (1 - sum) / circulation_information_.v_moving_level_density.size();
    t += i;
  }

  assert(circulation_information_.number_of_moving_levels == circulation_information_.v_moving_level_density.size());
  assert(circulation_information_.number_of_moving_levels == circulation_information_.v_moving_level_value.size());
  assert(fabs(t - 1) < 0.0001);

  circulation_information_.circulation_percent = n["circulation_percent"].as<double>();

  const auto length_of_stay_mean = n["length_of_stay"]["mean"].as<double>();
  const auto length_of_stay_sd = n["length_of_stay"]["sd"].as<double>();

  const auto stay_variance = length_of_stay_sd * length_of_stay_sd;
  const auto k = stay_variance / length_of_stay_mean; //k
  const auto theta = length_of_stay_mean / k; //theta

  circulation_information_.length_of_stay_theta = theta;
  circulation_information_.length_of_stay_k = k;
  moving_level_generator_.set_level_density(&circulation_information_.v_moving_level_density);

}

void Config::read_initial_parasite_info(const YAML::Node& config) {
  const YAML::Node& n = config["initial_parasite_info"];

  for (int i = 0; i < n.size(); i++) {
    auto location = n[i]["location_id"].as<int>();
    if (location < number_of_locations() && location != -1) {
      for (int j = 0; j < n[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        auto parasite_type_id = n[i]["parasite_info"][j]["parasite_type_id"].as<int>();
        auto prevalence = n[i]["parasite_info"][j]["prevalence"].as<double>();
        initial_parasite_info_.emplace_back(location, parasite_type_id, prevalence);
      }
    }
    else if (location == -1) {
      //apply for all location
      for (int loc = 0; loc < number_of_locations(); ++loc) {
        for (int j = 0; j < n[i]["parasite_info"].size(); j++) {
          //            InitialParasiteInfo ipi;
          //            ipi.location = location;
          auto parasite_type_id = n[i]["parasite_info"][j]["parasite_type_id"].as<int>();
          auto prevalence = n[i]["parasite_info"][j]["prevalence"].as<double>();
          initial_parasite_info_.emplace_back(loc, parasite_type_id, prevalence);
        }
      }
    }
  }
}

void Config::read_importation_parasite_info(const YAML::Node& config) {
  const YAML::Node& n = config["introduce_parasite"];

  for (int i = 0; i < n.size(); i++) {
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
        importation_parasite_periodically_info_.emplace_back(location, parasite_type_id, dur, num, start_day);
      }
    }
  }

}

void Config::read_relative_infectivity_info(const YAML::Node& config) {
  const YAML::Node& n = config["relative_infectivity"];

  relative_infectivity_.sigma = n["sigma"].as<double>();
  auto ro = n["ro"].as<double>();
  auto blood_meal_volume = n["blood_meal_volume"].as<double>();

  double d_star = 1 / blood_meal_volume;

  relative_infectivity_.ro_star = (log(ro) - log(d_star)) / relative_infectivity_.sigma;

  relative_infectivity_.sigma = log(10) / relative_infectivity_.sigma;
  //    std::cout << relative_infectivity_.sigma << "\t" << relative_infectivity_.ro_star<< std::endl;
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

    for (auto& i : genotype_info_.loci_vector) {
      for (auto& allele : i.alleles) {
        allele.daily_cost_of_resistance *= modified_daily_cost_of_resistance_;
      }
    }
    build_parasite_db();
  }

  if (parameter_name == "daily_cost_of_resistance_copies_pfmdr") {
    double dcr = atof(parameter_value.c_str());
    // modify allele 4 5 6 7 of the mdr locus
    genotype_info_.loci_vector[1].alleles[4].daily_cost_of_resistance = dcr;
    genotype_info_.loci_vector[1].alleles[5].daily_cost_of_resistance =
      1 - (1 - genotype_info_.loci_vector[1].alleles[1].daily_cost_of_resistance) * (1 - dcr);
    genotype_info_.loci_vector[1].alleles[6].daily_cost_of_resistance =
      1 - (1 - genotype_info_.loci_vector[1].alleles[2].daily_cost_of_resistance) * (1 - dcr);
    genotype_info_.loci_vector[1].alleles[7].daily_cost_of_resistance =
      1 - (1 - genotype_info_.loci_vector[1].alleles[3].daily_cost_of_resistance) * (1 - dcr);
    build_parasite_db();
  }

  if (parameter_name == "z") {
    immune_system_information_.immune_effect_on_progression_to_clinical = atof(parameter_value.c_str());
  }

  if (parameter_name == "kappa") {
    immune_system_information_.factor_effect_age_mature_immunity = atof(parameter_value.c_str());

    // implement inlation rate
    double acR = immune_system_information_.acquire_rate;
    immune_system_information_.acquire_rate_by_age.clear();
    for (int age = 0; age <= 80; age++) {
      double factor = 1;
      if (age < immune_system_information().age_mature_immunity) {
        factor = (age == 0) ? 0.5 : age;

        factor = factor / immune_system_information().age_mature_immunity;
        factor = pow(factor, immune_system_information().factor_effect_age_mature_immunity);
      }

      immune_system_information_.acquire_rate_by_age.push_back(factor * acR);

      acR *= (1 + immune_system_information_.immune_inflation_rate);
    }

    assert(immune_system_information_.acquire_rate_by_age.size() == 81);
  }

  if (parameter_name == "relative_bitting_sd") {
    relative_bitting_information_.sd = atof(parameter_value.c_str());
    calculate_relative_biting_density();
  }

  if (parameter_name == "k") {
    modified_mutation_factor_ = atof(parameter_value.c_str());
    for (auto& it : drug_db_->drug_db()) {
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
      ((NestedSwitchingStrategy *)strategy_)->initialize_update_time();
    }
  }

  if (parameter_name == "dosing_days") {
    int dosing_days = atoi(parameter_value.c_str());
    for (auto& it : therapy_db_) {

      auto* scTherapy = dynamic_cast<SCTherapy *>(it);
      if (scTherapy != nullptr) {
        scTherapy->set_dosing_day(dosing_days);
      }
    }
  }

  if (parameter_name == "mutation_probability") {
    modified_mutation_probability_ = atof(parameter_value.c_str());
    for (auto& it : drug_db_->drug_db()) {
      it.second->set_p_mutation(modified_mutation_probability_);

    }
    //        std::cout<< Model::CONFIG->drug_db()->drug_db().begin()->second->p_mutation() << std::endl;
  }

  if (parameter_name == "mutation_factor") {
    double mutation_factor = atof(parameter_value.c_str());
    for (auto& it : drug_db_->drug_db()) {
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


void Config::read_biodemography_information(const YAML::Node& config) {
  birth_rate_ = config["birth_rate"].as<double>();

  death_rate_by_age_.clear();
  for (int i = 0; i < number_of_age_classes(); i++) {
    death_rate_by_age_.push_back(config["death_rate_by_age"][i].as<double>());
  }

  mortality_when_treatment_fail_by_age_class_.clear();
  for (int i = 0; i < number_of_age_classes(); i++) {
    mortality_when_treatment_fail_by_age_class_.push_back(
      config["mortality_when_treatment_fail_by_age"][i].as<double>());
  }
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
