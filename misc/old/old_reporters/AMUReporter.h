/* 
 * File:   AMUReporter.h
 * Author: Merlin
 *
 * Created on January 20, 2014, 2:31 PM
 */

#ifndef AMUREPORTER_H
#define    AMUREPORTER_H

#include "Reporters/Reporter.h"

class AMUReporter : public Reporter {
 DISALLOW_COPY_AND_ASSIGN(AMUReporter)

 VIRTUAL_PROPERTY_REF(double, last_reported_NTF)

 public:

  AMUReporter();

  //    AMUReporter(const AMUReporter& orig);
  virtual ~AMUReporter();

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

  void print_popsize_by_age_class();

 private:

};

#endif    /* AMUREPORTER_H */

