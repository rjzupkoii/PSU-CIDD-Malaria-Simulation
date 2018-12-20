/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BurinFarmReporter.h
 * Author: Merlin
 *
 * Created on April 11, 2017, 1:45 PM
 */

#ifndef BURINFARMREPORTER_H
#define BURINFARMREPORTER_H

#include "Reporters/Reporter.h"
#include "Core/PropertyMacro.h"

class BurninFarmReporter : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(BurninFarmReporter)

 public:
  BurninFarmReporter();

  //    FarmReporter(const FarmReporter& orig);
  virtual ~BurninFarmReporter();

  virtual void initialize();

  virtual void before_run();

  virtual void after_run();

  virtual void begin_time_step();

  virtual void monthly_report();

  void output_parameters();

  void print_ntf_by_location();

  void print_EIR_by_location();

  void print_20x_by_location();

  void print_EAMU();

  void print_prevalence_by_location();

  void print_fraction_of_positive_that_are_clinical_by_location();

  void print_mean_immune_by_location();

  void print_LOI();

  void print_mean_number_of_clones_per_positive_individual();

  void print_mean_number_of_clones_per_positive_individual_by_age_group();

  void print_death_by_age_group();

  void print_number_of_clinical_episode_by_age_class();

  void print_prevalence_by_age_class();

  void print_fraction_of_positive_that_are_clinical_by_location_age_class();

  void print_fraction_of_positive_that_are_clinical_by_location_age_class_by_5();

  void print_resistance_tracker();

  void print_treatments_by_therapy();

  void print_total_utl();

  void print_utl();

  void print_mean_immune_by_location_age_class();

  void print_moi_distribution();

 private:

};

#endif /* BURINFARMREPORTER_H */

