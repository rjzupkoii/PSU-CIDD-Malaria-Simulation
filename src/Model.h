/* 
 * File:   Model.h
 * Author: nguyentran
 *
 * Created on March 22, 2013, 2:26 PM
 */

#ifndef MODEL_H
#define    MODEL_H

#include <vector>
#include "Core/PropertyMacro.h"
#include "Core/Scheduler.h"
#include "Population/ClinicalUpdateFunction.h"
#include "Population/ImmunityClearanceUpdateFunction.h"
#include "Malaria/ITreatmentCoverageModel.h"

class Scheduler;

class Population;

class Config;

class Random;

class ModelDataCollector;

class Reporter;

class Model {
 DISALLOW_COPY_AND_ASSIGN(Model)

 DISALLOW_MOVE(Model)

 POINTER_PROPERTY(Config, config)

 POINTER_PROPERTY(Scheduler, scheduler)

 POINTER_PROPERTY(Population, population)

 POINTER_PROPERTY(Random, random)

 POINTER_PROPERTY(ModelDataCollector, data_collector)

 POINTER_PROPERTY(ClinicalUpdateFunction, progress_to_clinical_update_function)

 POINTER_PROPERTY(ImmunityClearanceUpdateFunction, immunity_clearance_update_function)

 POINTER_PROPERTY(ImmunityClearanceUpdateFunction, having_drug_update_function)

 POINTER_PROPERTY(ImmunityClearanceUpdateFunction, clinical_update_function)

 PROPERTY_REF(std::vector<Reporter *>, reporters)

 PROPERTY_REF(unsigned long, initial_seed_number)

 PROPERTY_REF(std::string, config_filename)

 PROPERTY_REF(int, cluster_job_number)

 PROPERTY_REF(std::string, tme_filename)

 PROPERTY_REF(std::string, override_parameter_filename)

 PROPERTY_REF(int, override_parameter_line_number) // base 1
 PROPERTY_REF(int, gui_type)

 PROPERTY_REF(bool, is_farm_output)

 PROPERTY_REF(std::string, reporter_type)

 public:
  static Model *MODEL;
  static Config *CONFIG;
  static Random *RANDOM;
  static Scheduler *SCHEDULER;
  static ModelDataCollector *DATA_COLLECTOR;
  static Population *POPULATION;

  static IStrategy *TREATMENT_STRATEGY;
  static ITreatmentCoverageModel *TREATMENT_COVERAGE;
  // static std::shared_ptr<spdlog::logger> LOGGER;

  explicit Model(const int &object_pool_size = 100000);

  virtual ~Model();

  void set_treatment_strategy(const int &strategy_id);

  void set_treatment_coverage(ITreatmentCoverageModel *tcm);

  void build_initial_treatment_coverage();

  void initialize();

  static void initialize_object_pool(const int &size = 100000);

  static void release_object_pool();

  void before_run();

  void run();

  void after_run();

  void begin_time_step();

  void perform_population_events_daily() const;

  void daily_update(const int &current_time);

  void monthly_update();

  void yearly_update();

  void release();

  void report_begin_of_time_step();

  void monthly_report();

  void add_reporter(Reporter *reporter);

  double get_seasonal_factor(const date::sys_days &today, const int &location) const;

 private:
  IStrategy *treatment_strategy_{nullptr};
  ITreatmentCoverageModel *treatment_coverage_{nullptr};

};

#endif    /* MODEL_H */
