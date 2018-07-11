/* 
 * File:   Config.h
 * Author: nguyentran
 *
 * Created on March 27, 2013, 10:38 AM
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "DependentConfigItem.h"
#include "ConfigItem.h"
#include "DrugDatabase.h"
#include "IntGenotypeDatabase.h"
#include "Spatial/Location.h"
#include "Spatial/SpatialModel.h"
#include "MultinomialDistributionGenerator.h"
#include <string>
#include <vector>
#include <date/date.h>
#include <chrono>

class IConfigItem;

class Model;


class Config {
DISALLOW_COPY_AND_ASSIGN(Config)
DISALLOW_MOVE(Config)

public:
POINTER_PROPERTY(Model, model)
  std::vector<IConfigItem*> config_items{};

  CONFIG_ITEM(starting_date, date::year_month_day, date::year_month_day{ date::year{ 1999 } / 1 / 1 })
  CONFIG_ITEM(ending_date, date::year_month_day, date::year_month_day{ date::year{ 1999 } / 1 / 2 })

  DEPENDENT_CONFIG_ITEM(total_time, 100)

  CONFIG_ITEM(start_treatment_day, int, 0)
  CONFIG_ITEM(start_collect_data_day, int, 0)
  CONFIG_ITEM(start_intervention_day, int, 0)

  CONFIG_ITEM(number_of_tracking_days, int, 0)
  CONFIG_ITEM(p_infection_from_an_infectious_bite, double, 0.0)

  CONFIG_ITEM(age_structure, std::vector<int>, std::vector<int>{ (1,2,3,4,5) })
  CONFIG_ITEM(initial_age_structure, std::vector<int>, std::vector<int>{ (1,2,3,4,5) })
  DEPENDENT_CONFIG_ITEM(number_of_age_classes, 0)
  // TODO: test
  CONFIG_ITEM(location_db, std::vector<Spatial::Location>, std::vector<Spatial::Location>{Spatial::Location(0,0,0,10000)})
  DEPENDENT_CONFIG_ITEM(number_of_locations, 1)

  DEPENDENT_CONFIG_ITEM(spatial_distance_matrix, DoubleVector2())

  DEPENDENT_CONFIG_ITEM(seasonal_info, SeasonalInfo())


POINTER_PROPERTY(Spatial::SpatialModel, spatial_model)

VIRTUAL_PROPERTY_REF(int, number_of_parasite_types)


VIRTUAL_PROPERTY_REF(DoubleVector2, EC50_power_n_table)


VIRTUAL_PROPERTY_REF(double, birth_rate)

VIRTUAL_PROPERTY_REF(DoubleVector, death_rate_by_age)


VIRTUAL_PROPERTY_REF(DoubleVector, mortality_when_treatment_fail_by_age_class)

VIRTUAL_PROPERTY_REF(ParasiteDensityLevel, log_parasite_density_level)

VIRTUAL_PROPERTY_REF(ImmuneSystemInformation, immune_system_information)

VIRTUAL_PROPERTY_REF(RelativeBittingInformation, relative_bitting_information)
VIRTUAL_PROPERTY_REF(RelativeInfectivity, relative_infectivity)

POINTER_PROPERTY(IStrategy, strategy)
POINTER_PROPERTY(DrugDatabase, drug_db)
POINTER_PROPERTY(IntGenotypeDatabase, genotype_db)
VIRTUAL_PROPERTY_REF(GenotypeInfo, genotype_info)

VIRTUAL_PROPERTY_REF(TherapyPtrVector, therapy_db)
VIRTUAL_PROPERTY_REF(StrategyPtrVector, strategy_db)

VIRTUAL_PROPERTY_REF(std::vector<InitialParasiteInfo>, initial_parasite_info)

VIRTUAL_PROPERTY_REF(std::vector<ImportationParasiteInfo>, importation_parasite_info)

VIRTUAL_PROPERTY_REF(std::vector<ImportationParasitePeriodicallyInfo>, importation_parasite_periodically_info)

VIRTUAL_PROPERTY_REF(int, days_to_clinical_under_five)
VIRTUAL_PROPERTY_REF(int, days_to_clinical_over_five)
VIRTUAL_PROPERTY_REF(int, days_mature_gametocyte_under_five)
VIRTUAL_PROPERTY_REF(int, days_mature_gametocyte_over_five)

VIRTUAL_PROPERTY_REF(double, p_compliance)
VIRTUAL_PROPERTY_REF(int, min_dosing_days)

VIRTUAL_PROPERTY_REF(double, gametocyte_level_under_artemisinin_action)
VIRTUAL_PROPERTY_REF(double, gametocyte_level_full)

VIRTUAL_PROPERTY_REF(double, p_relapse)
VIRTUAL_PROPERTY_REF(int, relapse_duration)

VIRTUAL_PROPERTY_REF(bool, allow_new_coinfection_to_cause_symtoms)
VIRTUAL_PROPERTY_REF(int, update_frequency)
VIRTUAL_PROPERTY_REF(int, report_frequency)

VIRTUAL_PROPERTY_REF(RelativeMovingInformation, circulation_information)

VIRTUAL_PROPERTY_REF(double, TF_rate)

VIRTUAL_PROPERTY_REF(TMEInfo, tme_info)

POINTER_PROPERTY(IStrategy, tme_strategy)
VIRTUAL_PROPERTY_REF(double, modified_mutation_factor)

VIRTUAL_PROPERTY_REF(double, modified_drug_half_life)

VIRTUAL_PROPERTY_REF(bool, using_free_recombination)

VIRTUAL_PROPERTY_REF(int, tf_testing_day)

VIRTUAL_PROPERTY_REF(int, tf_window_size)

VIRTUAL_PROPERTY_REF(bool, using_age_dependent_bitting_level)

VIRTUAL_PROPERTY_REF(bool, using_variable_probability_infectious_bites_cause_infection)

VIRTUAL_PROPERTY_REF(double, fraction_mosquitoes_interrupted_feeding)


VIRTUAL_PROPERTY_REF(double, modified_daily_cost_of_resistance)

VIRTUAL_PROPERTY_REF(double, modified_mutation_probability)

VIRTUAL_PROPERTY_REF(double, inflation_factor)

READ_ONLY_PROPERTY_REF(MultinomialDistributionGenerator, bitting_level_generator)
READ_ONLY_PROPERTY_REF(MultinomialDistributionGenerator, moving_level_generator)

public:
  explicit Config(Model* model = nullptr);

  virtual ~Config();

  void read_from_file(const std::string& config_file_name = "config.yml");

  void read_immune_system_information(const YAML::Node& config);

  void read_parasite_density_level(const YAML::Node& config);

  void read_strategy_therapy_and_drug_information(const YAML::Node& config);

  void read_relative_biting_rate_info(const YAML::Node& config);

  void calculate_relative_biting_density();

  void read_spatial_info(const YAML::Node& config);
  
  void read_initial_parasite_info(const YAML::Node& config);

  void read_importation_parasite_info(const YAML::Node& config);

  void read_importation_parasite_periodically_info(const YAML::Node& config);

  void read_relative_infectivity_info(const YAML::Node& config);

  IStrategy* read_strategy(const YAML::Node& n, const int& strategy_id) const;

  Therapy* read_therapy(const YAML::Node& n, const int& therapy_id) const;

  DrugType* read_drugtype(const YAML::Node& config, const int& drug_id) const;

  void override_parameters(const std::string& override_file, const int& pos);

  void override_1_parameter(const std::string& parameter_name, const std::string& parameter_value);

  void build_drug_db(const YAML::Node& config);

  void build_parasite_db();

  void build_drug_and_parasite_db(const YAML::Node& config);

  void read_genotype_info(const YAML::Node& config);

  void read_biodemography_information(const YAML::Node& config);

  double get_seasonal_factor(const date::sys_days& today, const int& location) const;
};

#endif /* CONFIG_H */
