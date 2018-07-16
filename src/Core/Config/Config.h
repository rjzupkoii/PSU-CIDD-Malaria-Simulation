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
#include "CustomConfigItem.h"
#include "ConfigItem.h"
#include "Spatial/Location.h"
#include "Core/MultinomialDistributionGenerator.h"
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

CUSTOM_CONFIG_ITEM(total_time, 100)

CONFIG_ITEM(start_treatment_day, int, 0)
CONFIG_ITEM(start_collect_data_day, int, 0)
CONFIG_ITEM(start_intervention_day, int, 0)

CONFIG_ITEM(number_of_tracking_days, int, 0)
CONFIG_ITEM(p_infection_from_an_infectious_bite, double, 0.0)

CONFIG_ITEM(age_structure, std::vector<int>, std::vector<int>{ (1, 2, 3, 4, 5) })
CONFIG_ITEM(initial_age_structure, std::vector<int>, std::vector<int>{ (1, 2, 3, 4, 5) })

CONFIG_ITEM(tf_testing_day, int, 28)

CONFIG_ITEM(days_to_clinical_under_five, int, 4)
CONFIG_ITEM(days_to_clinical_over_five, int, 6)
CONFIG_ITEM(days_mature_gametocyte_under_five, int, 4)
CONFIG_ITEM(days_mature_gametocyte_over_five, int, 6)

CONFIG_ITEM(p_compliance, double, 1.0)
CONFIG_ITEM(min_dosing_days, int, 1)

CONFIG_ITEM(gametocyte_level_full, double, 1.0)

CONFIG_ITEM(p_relapse, double, 0.01)
CONFIG_ITEM(relapse_duration, int, 30)

CONFIG_ITEM(allow_new_coinfection_to_cause_symtoms, bool, true)
CONFIG_ITEM(update_frequency, int, 7)
CONFIG_ITEM(report_frequency, int, 30)

CONFIG_ITEM(TF_rate, double, 0.1)
CONFIG_ITEM(using_free_recombination, bool, true)
CONFIG_ITEM(tf_window_size, int, 60)

CONFIG_ITEM(using_age_dependent_bitting_level, bool, false)
CONFIG_ITEM(using_variable_probability_infectious_bites_cause_infection, bool, false)

CONFIG_ITEM(fraction_mosquitoes_interrupted_feeding, double, 0.0)
CONFIG_ITEM(inflation_factor, double, 1.01)

CONFIG_ITEM(location_db, std::vector<Spatial::Location>, std::vector<Spatial::Location>{Spatial::Location(0, 0, 0, 10000)})

CONFIG_ITEM(birth_rate, double, 0)

CONFIG_ITEM(death_rate_by_age_class, DoubleVector, DoubleVector())

CONFIG_ITEM(mortality_when_treatment_fail_by_age_class, DoubleVector, DoubleVector())

CONFIG_ITEM(parasite_density_level, ParasiteDensityLevel, ParasiteDensityLevel())

CONFIG_ITEM(relative_infectivity, RelativeInfectivity, RelativeInfectivity())

CONFIG_ITEM(genotype_info, GenotypeInfo, GenotypeInfo())


CUSTOM_CONFIG_ITEM(number_of_age_classes, 0)

CUSTOM_CONFIG_ITEM(number_of_locations, 1)

CUSTOM_CONFIG_ITEM(spatial_distance_matrix, DoubleVector2())

CUSTOM_CONFIG_ITEM(seasonal_info, SeasonalInfo())

CUSTOM_CONFIG_ITEM(spatial_model, nullptr)

CUSTOM_CONFIG_ITEM(immune_system_information, ImmuneSystemInformation())

CUSTOM_CONFIG_ITEM(genotype_db, nullptr)

CUSTOM_CONFIG_ITEM(number_of_parasite_types, 0)

CUSTOM_CONFIG_ITEM(drug_db, nullptr)

CUSTOM_CONFIG_ITEM(EC50_power_n_table, DoubleVector2())

CUSTOM_CONFIG_ITEM(circulation_info, RelativeMovingInformation())

CUSTOM_CONFIG_ITEM(relative_bitting_info, RelativeBittingInformation())

CUSTOM_CONFIG_ITEM(therapy_db, TherapyPtrVector())

CUSTOM_CONFIG_ITEM(strategy_db, StrategyPtrVector())

CUSTOM_CONFIG_ITEM(strategy, nullptr)



VIRTUAL_PROPERTY_REF(std::vector<InitialParasiteInfo>, initial_parasite_info)

VIRTUAL_PROPERTY_REF(std::vector<ImportationParasiteInfo>, importation_parasite_info)

VIRTUAL_PROPERTY_REF(std::vector<ImportationParasitePeriodicallyInfo>, importation_parasite_periodically_info)


VIRTUAL_PROPERTY_REF(double, modified_mutation_factor)

VIRTUAL_PROPERTY_REF(double, modified_drug_half_life)

VIRTUAL_PROPERTY_REF(double, modified_daily_cost_of_resistance)

VIRTUAL_PROPERTY_REF(double, modified_mutation_probability)


READ_ONLY_PROPERTY_REF(MultinomialDistributionGenerator, bitting_level_generator)
READ_ONLY_PROPERTY_REF(MultinomialDistributionGenerator, moving_level_generator)

public:
  explicit Config(Model* model = nullptr);

  virtual ~Config();

  void read_from_file(const std::string& config_file_name = "config.yml");

  void read_initial_parasite_info(const YAML::Node& config);

  void read_importation_parasite_info(const YAML::Node& config);

  void read_importation_parasite_periodically_info(const YAML::Node& config);
  
  void override_parameters(const std::string& override_file, const int& pos);

  void override_1_parameter(const std::string& parameter_name, const std::string& parameter_value);

  double get_seasonal_factor(const date::sys_days& today, const int& location) const;
};

#endif /* CONFIG_H */
