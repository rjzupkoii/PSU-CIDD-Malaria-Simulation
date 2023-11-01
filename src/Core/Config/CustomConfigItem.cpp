/*
 * CustomConfigItem.cpp
 * 
 * Implement the custom configuration items.
 */
#include "CustomConfigItem.h"

#include <algorithm>
#include <cmath>
#include <date/date.h>
#include <gsl/gsl_cdf.h>

#include "Config.h"
#include "Helpers/NumberHelpers.h"
#include "Helpers/ObjectHelpers.h"
#include "Spatial/SpatialModelBuilder.hxx"
#include "Strategies/IStrategy.h"
#include "Strategies/StrategyBuilder.h"
#include "Therapies/Therapy.hxx"
#include "Therapies/TherapyBuilder.h"

void total_time::set_value(const YAML::Node &node) {
  value_ = (date::sys_days{config_->ending_date()} - date::sys_days(config_->starting_date())).count();
}

void number_of_age_classes::set_value(const YAML::Node &node) {
  value_ = static_cast<int>(config_->age_structure().size());
}

void number_of_locations::set_value() {
  value_ = static_cast<int>(config_->location_db().size());
}

void number_of_locations::set_value(const YAML::Node &node) {
  set_value();
}

seasonal_info::~seasonal_info() {
  if (value_ != nullptr) {
    delete value_;
    value_ = nullptr;
  }
}

void seasonal_info::set_value(const YAML::Node &node) {
  value_ = SeasonalInfoFactory::build(node[name_], config_);
}

spatial_model::~spatial_model() {
  if (value_ != nullptr) {
    delete value_;
    value_ = nullptr;
  }
}

void spatial_model::set_value(const YAML::Node &node) {
  try {
    const auto sm_name = node[name_]["name"].as<std::string>();
    value_ = Spatial::SpatialModelBuilder::Build(sm_name, node[name_][sm_name]);
    VLOG(1) << "Using spatial model: " << sm_name;
  } catch (YAML::InvalidNode &ex) {
    throw std::invalid_argument("Spatial model does not contain a name node.");
  }
}

void immune_system_information::set_value(const YAML::Node &node) {
  auto is_node = node[name_];  
  value_.acquire_rate = read_node<double>(is_node, "b1");
  value_.decay_rate = read_node<double>(is_node, "b2");

  value_.duration_for_fully_immune = read_node<double>(is_node, "duration_for_fully_immune");
  value_.duration_for_naive = read_node<double>(is_node, "duration_for_naive");

  const auto mean_initial_condition = read_node<double>(is_node, "mean_initial_condition");
  const auto sd_initial_condition = read_node<double>(is_node, "sd_initial_condition");

  if (NumberHelpers::is_equal(sd_initial_condition, 0.0)) {
    value_.alpha_immune = mean_initial_condition;
    value_.beta_immune = 0.0;
  } else {
    value_.alpha_immune =
      mean_initial_condition * mean_initial_condition * (1 - mean_initial_condition) /
      (sd_initial_condition * sd_initial_condition) - mean_initial_condition;
    value_.beta_immune = value_.alpha_immune / mean_initial_condition -
                         value_.alpha_immune;
  }

  value_.immune_inflation_rate = read_node<double>(is_node, "immune_inflation_rate");

  value_.max_clinical_probability = read_node<double>(is_node, "max_clinical_probability");

  value_.immune_effect_on_progression_to_clinical = read_node<double>(is_node, "immune_effect_on_progression_to_clinical");

  value_.age_mature_immunity = read_node<double>(is_node, "age_mature_immunity");
  value_.factor_effect_age_mature_immunity = read_node<double>(is_node, "factor_effect_age_mature_immunity");

  value_.midpoint = read_node<double>(is_node, "midpoint");

  // implement inflation rate
  double acR = value_.acquire_rate;
  value_.acquire_rate_by_age.clear();
  for (int age = 0; age <= 80; age++) {
    double factor = 1;
    if (age < value_.age_mature_immunity) {
      factor = (age == 0) ? 0.5 : age;

      factor = factor / value_.age_mature_immunity;
      factor = pow(factor, value_.factor_effect_age_mature_immunity);
    }

    value_.acquire_rate_by_age.push_back(factor * acR);

    acR *= (1 + value_.immune_inflation_rate);
  }
  assert(value_.acquire_rate_by_age.size() == 81);

  value_.c_min = pow(10, -(config_->parasite_density_level().log_parasite_density_asymptomatic -
                           config_->parasite_density_level().log_parasite_density_cured) /
                         value_.duration_for_fully_immune);
  value_.c_max = pow(10, -(config_->parasite_density_level().log_parasite_density_asymptomatic -
                           config_->parasite_density_level().log_parasite_density_cured) /
                         value_.duration_for_naive);
}

genotype_db::~genotype_db() {
  ObjectHelpers::delete_pointer<GenotypeDatabase>(value_);
}

void genotype_db::set_value(const YAML::Node &node) {

  value_ = new GenotypeDatabase();

  value_->weight().clear();
  value_->weight().assign(config_->genotype_info().loci_vector.size(), 1);

  auto temp = 1;
  for (int i = static_cast<int>(value_->weight().size() - 2); i >= 0; i--) {
    temp *= (int) config_->genotype_info().loci_vector[i + 1].alleles.size();
    value_->weight()[i] = temp;
  }
  auto number_of_genotypes = 1;
  for (auto &locus : config_->genotype_info().loci_vector) {
    number_of_genotypes *= (int) locus.alleles.size();
  }

  for (auto i = 0; i < number_of_genotypes; i++) {
    auto* int_genotype = new Genotype(i, config_->genotype_info(), value_->weight());
    value_->add(int_genotype);
  }

  value_->initialize_matting_matrix();

}

void number_of_parasite_types::set_value(const YAML::Node &node) {
  value_ = static_cast<int>(config_->genotype_db()->size());
}

drug_db::~drug_db() {
  ObjectHelpers::delete_pointer<DrugDatabase>(value_);
}

void drug_db::set_value(const YAML::Node &node) {
  ObjectHelpers::delete_pointer<DrugDatabase>(value_);
  value_ = new DrugDatabase();

  for (auto drug_id = 0; drug_id < node[name_].size(); drug_id++) {
    auto* dt = new DrugType();
    dt->set_id(drug_id);

    const auto i_s = NumberHelpers::number_to_string<int>(drug_id);
    const auto &dt_node = node[name_][i_s];

    dt->set_name(dt_node["name"].as<std::string>());
    dt->set_drug_half_life(dt_node["half_life"].as<double>());
    dt->set_maximum_parasite_killing_rate(dt_node["maximum_parasite_killing_rate"].as<double>());
    dt->set_n(dt_node["n"].as<double>());

    for (std::size_t i = 0; i < dt_node["age_specific_drug_concentration_sd"].size(); i++) {
      dt->age_group_specific_drug_concentration_sd().push_back(
        dt_node["age_specific_drug_concentration_sd"][i].as<double>());
      dt->age_specific_drug_absorption().push_back(1.0);
    }

    if ( dt_node["age_specific_drug_absorption"]){
      for (std::size_t i = 0; i < dt_node["age_specific_drug_absorption"].size(); i++) {
        dt->age_specific_drug_absorption()[i] = dt_node["age_specific_drug_absorption"][i].as<double>();
      }
    }

    dt->set_p_mutation(dt_node["mutation_probability"].as<double>());

    dt->affecting_loci().clear();
    for (std::size_t i = 0; i < dt_node["affecting_loci"].size(); i++) {
      dt->affecting_loci().push_back(dt_node["affecting_loci"][i].as<int>());
    }

    dt->selecting_alleles().clear();
    dt->selecting_alleles().assign(dt_node["affecting_loci"].size(), IntVector());
    for (std::size_t i = 0; i < dt_node["affecting_loci"].size(); i++) {
      for (std::size_t j = 0; j < dt_node["selecting_alleles"][i].size(); j++) {
        dt->selecting_alleles()[i].push_back(dt_node["selecting_alleles"][i][j].as<int>());

      }
    }

    dt->set_ec50_map(dt_node["EC50"].as<std::map<std::string, double>>());

    dt->set_k(dt_node["k"].as<double>());

    value_->add(dt);

  }
}

void EC50_power_n_table::set_value(const YAML::Node &node) {
  //get EC50 table and compute EC50^n
  value_.clear();
  value_.assign(config_->genotype_db()->size(), std::vector<double>());

  for (unsigned int g_id = 0; g_id < config_->genotype_db()->size(); g_id++) {
    for (auto i = 0; i < config_->drug_db()->size(); i++) { // NOLINT(modernize-loop-convert)
      value_[g_id].push_back(config_->drug_db()->at(i)->infer_ec50(config_->genotype_db()->at(g_id)));
    }
  }


  for (unsigned int g_id = 0; g_id < config_->genotype_db()->size(); g_id++) {
    for (auto i = 0; i < config_->drug_db()->size(); i++) {
      value_[g_id][i] = pow(value_[g_id][i], config_->drug_db()->at(i)->n());
    }
  }
}

void circulation_info::set_value(const YAML::Node &node) {
  auto info_node = node[name_];
  value_.max_relative_moving_value = info_node["max_relative_moving_value"].as<double>();

  value_.number_of_moving_levels = info_node["number_of_moving_levels"].as<int>();

  value_.scale = info_node["moving_level_distribution"]["Exponential"]["scale"].as<double>();

  value_.mean = info_node["moving_level_distribution"]["Gamma"]["mean"].as<double>();
  value_.sd = info_node["moving_level_distribution"]["Gamma"]["sd"].as<double>();

  //calculate density and level value here

  const auto var = value_.sd * value_.sd;

  const auto b = var / (value_.mean - 1); //theta
  const auto a = (value_.mean - 1) / b; //k

  value_.v_moving_level_density.clear();
  value_.v_moving_level_value.clear();

  const auto max = value_.max_relative_moving_value - 1; //maxRelativeBiting -1
  const auto number_of_level = value_.number_of_moving_levels;

  const auto step = max / static_cast<double>(number_of_level - 1);

  auto j = 0;
  double old_p = 0;
  double sum = 0;
  for (double i = 0; i <= max + 0.0001; i += step) {
    const auto p = gsl_cdf_gamma_P(i + step, a, b);
    auto value = (j == 0) ? p : p - old_p;
    value_.v_moving_level_density.push_back(value);
    old_p = p;
    value_.v_moving_level_value.push_back(i + 1);
    sum += value;
    j++;

  }

  //normalized
  double t = 0;
  for (auto &i : value_.v_moving_level_density) {
    i = i + (1 - sum) / static_cast<double>(value_.v_moving_level_density.size());
    t += i;
  }

  assert((unsigned)value_.number_of_moving_levels == value_.v_moving_level_density.size());
  assert((unsigned)value_.number_of_moving_levels == value_.v_moving_level_value.size());
  assert(fabs(t - 1) < 0.0001);

  value_.circulation_percent = info_node["circulation_percent"].as<double>();

  const auto length_of_stay_mean = info_node["length_of_stay"]["mean"].as<double>();
  const auto length_of_stay_sd = info_node["length_of_stay"]["sd"].as<double>();

  const auto stay_variance = length_of_stay_sd * length_of_stay_sd;
  const auto k = stay_variance / length_of_stay_mean; //k
  const auto theta = length_of_stay_mean / k; //theta

  value_.length_of_stay_mean = length_of_stay_mean;
  value_.length_of_stay_sd = length_of_stay_sd;
  value_.length_of_stay_theta = theta;
  value_.length_of_stay_k = k;
}

void relative_bitting_info::set_value(const YAML::Node &node) {
  auto info_node = node[name_];
  value_.max_relative_biting_value = info_node["max_relative_biting_value"].as<double>();

  value_.number_of_biting_levels = info_node["number_of_biting_levels"].as<int>();

  value_.scale = info_node["biting_level_distribution"]["Exponential"]["scale"].as<double>();

  value_.mean = info_node["biting_level_distribution"]["Gamma"]["mean"].as<double>();
  value_.sd = info_node["biting_level_distribution"]["Gamma"]["sd"].as<double>();

  const auto var = value_.sd * value_.sd;
  const auto b = var / (value_.mean - 1); //theta
  const auto a = (value_.mean - 1) / b; //k

  value_.v_biting_level_density.clear();
  value_.v_biting_level_value.clear();

  const auto max = value_.max_relative_biting_value - 1; //maxRelativeBiting -1
  const auto number_of_level = value_.number_of_biting_levels;

  const auto step = max / static_cast<double>(number_of_level - 1);

  auto j = 0;
  auto old_p = 0.0;
  auto sum = 0.0;

  for (double i = 0; i <= max + 0.0001; i += step) {
    const auto p = gsl_cdf_gamma_P(i + step, a, b);
    auto value = (j == 0) ? p : p - old_p;
    value_.v_biting_level_density.push_back(value);
    old_p = p;
    value_.v_biting_level_value.push_back(i + 1);
    sum += value;
    j++;

  }


  //normalized
  double t = 0;
  for (auto &i : value_.v_biting_level_density) {
    i = i + (1 - sum) / static_cast<double>(value_.v_biting_level_density.size());
    t += i;
  }

  assert((unsigned)value_.number_of_biting_levels == value_.v_biting_level_density.size());
  assert((unsigned)value_.number_of_biting_levels == value_.v_biting_level_value.size());
  assert(fabs(t - 1) < 0.0001);

}

therapy_db::~therapy_db() {
  for (auto &i : value_) {
    delete i;
  }
  value_.clear();
}

Therapy* read_therapy(const YAML::Node &n, const int &therapy_id) {
  const auto t_id = NumberHelpers::number_to_string<int>(therapy_id);
  auto* t = TherapyBuilder::build(n[t_id], therapy_id);
  return t;
}

void therapy_db::set_value(const YAML::Node &node) {
  //    read_all_therapy
  for (std::size_t i = 0; i < node[name_].size(); i++) {
    auto* t = read_therapy(node[name_], (int) i);
    value_.push_back(t);
  }
}

strategy_db::~strategy_db() {
  for (auto &i : value_) {
    delete i;
  }
  value_.clear();
}

IStrategy* read_strategy(const YAML::Node &n, const int &strategy_id, Config* config) {
  const auto s_id = NumberHelpers::number_to_string<int>(strategy_id);
  auto* result = StrategyBuilder::build(n[s_id], strategy_id, config);
  VLOG(9) << fmt::format("Strategy: {0}", result->to_string());
  return result;
}

void strategy_db::set_value(const YAML::Node &node) {
  for (std::size_t i = 0; i < node[name_].size(); i++) {
    auto* s = read_strategy(node[name_], (int) i, config_);
    value_.push_back(s);
  }
}

void initial_parasite_info::set_value(const YAML::Node &node) {

  const auto &info_node = node[name_];

  for (const auto & index : info_node) {
    const auto location = index["location_id"].as<int>();
    const auto location_from = location == -1 ? 0 : location;
    const auto location_to = location == -1 ? config_->number_of_locations() : std::min<int>(location + 1, static_cast<int>(config_->number_of_locations()));

    //apply for all location, note that the compiler has a hard time guessing the right type for auto here
    for (unsigned long loc = location_from; loc < location_to; ++loc) {
      for (std::size_t j = 0; j < index["parasite_info"].size(); j++) {
        auto parasite_type_id = index["parasite_info"][j]["parasite_type_id"].as<int>();
        auto prevalence = index["parasite_info"][j]["prevalence"].as<double>();
        value_.emplace_back(loc, parasite_type_id, prevalence);
      }
    }

  }
}

void bitting_level_generator::set_value(const YAML::Node &node) {
  value_.level_density = config_->relative_bitting_info().v_biting_level_density;
}

void moving_level_generator::set_value(const YAML::Node &node) {
  value_.level_density = config_->circulation_info().v_moving_level_density;
}

void start_of_comparison_period::set_value(const YAML::Node &node) {
  const auto ymd = node[name_].as<date::year_month_day>();
  value_ = (date::sys_days{ymd} - date::sys_days(config_->starting_date())).count();
}

void prob_individual_present_at_mda_distribution::set_value(const YAML::Node &node) {
  value_.clear();
  for (std::size_t i = 0; i < config_->mean_prob_individual_present_at_mda().size(); i++) {
    const auto mean = config_->mean_prob_individual_present_at_mda()[i];
    const auto sd = config_->sd_prob_individual_present_at_mda()[i];

    beta_distribution_params params{};

    if (NumberHelpers::is_equal(sd, 0.0)) {
      params.alpha = mean;
      params.beta = 0.0;
    } else {
      params.alpha = mean * mean * (1 - mean) / (sd * sd) - mean;
      params.beta = params.alpha / mean - params.alpha;
    }

    value_.push_back(params);
  }
}
