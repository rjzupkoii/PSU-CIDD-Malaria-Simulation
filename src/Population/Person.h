/* 
 * Person.h
 * 
 * Define the person class and the methods that are associated with it.
 */

#ifndef PERSON_H
#define PERSON_H

#include "Core/PropertyMacro.h"
#include "Properties/PersonIndexAllHandler.hxx"
#include "Properties/PersonIndexByLocationStateAgeClassHandler.hxx"
#include "Core/ObjectPool.h"
#include "Core/Dispatcher.h"
#include "Events/Event.h"
#include "Properties/PersonIndexByLocationBitingLevelHandler.hxx"
#include "Properties/PersonIndexByLocationMovingLevelHandler.hxx"
#include "ClonalParasitePopulation.h"
#include "Helpers/UniqueId.hxx"
#include "Therapies/SCTherapy.h"

class Population;

class ImmuneSystem;

class SingleHostClonalParasitePopulations;

class ClonalParasitePopulation;

class Genotype;

class Event;

class ParasiteDensityUpdateFunction;

class DrugType;

class Scheduler;

class Model;

class Config;

class ModelDataCollector;

class Random;

class DrugsInBlood;

class Genotype;

class Person : public PersonIndexAllHandler, public PersonIndexByLocationStateAgeClassHandler,
               public PersonIndexByLocationBitingLevelHandler, public PersonIndexByLocationMovingLevelHandler,
               public Dispatcher {
 public:

  enum Property {
    LOCATION = 0,
    HOST_STATE,
    AGE,
    AGE_CLASS,
    BITING_LEVEL,
    MOVING_LEVEL,
    EXTERNAL_POPULATION_MOVING_LEVEL
  };

  enum HostStates {
    SUSCEPTIBLE = 0,
    EXPOSED = 1,
    ASYMPTOMATIC = 2,
    CLINICAL = 3,
    DEAD = 4,
    NUMBER_OF_STATE = 5
  };

 OBJECTPOOL(Person)

 DISALLOW_COPY_AND_ASSIGN(Person)

 DISALLOW_MOVE(Person)

 POINTER_PROPERTY(Population, population)

 PROPERTY_HEADER(int, location)

 PROPERTY_REF(int, residence_location)

 PROPERTY_HEADER(HostStates, host_state)

 PROPERTY_HEADER(int, age)

 PROPERTY_HEADER(int, age_class)

  // birthday has the unit of time in the scheduler
  // if birthday is -100 which is that person was born 100 day before the simulation start
 PROPERTY_REF(int, birthday)

 POINTER_PROPERTY(SingleHostClonalParasitePopulations, all_clonal_parasite_populations)

 VIRTUAL_PROPERTY_REF(int, latest_update_time)

 PROPERTY_HEADER(int, biting_level)

 PROPERTY_REF(double, base_biting_level_value)

 PROPERTY_HEADER(int, moving_level)

 POINTER_PROPERTY(DrugsInBlood, drugs_in_blood)

 POINTER_PROPERTY(Genotype, liver_parasite_type)

 POINTER_PROPERTY(IntVector, today_infections)

 POINTER_PROPERTY(IntVector, today_target_locations)

 PROPERTY_REF(int, number_of_times_bitten)

 PROPERTY_REF(int, number_of_trips_taken)

 PROPERTY_REF(int, last_therapy_id)

 PROPERTY_REF(std::vector<double>, prob_present_at_mda_by_age)

private:
  // The UID is generated each time the person is initialized
  ul_uid _uid = -1;

  // Formally set via macro, but we don't want to replace an immune system
  ImmuneSystem* immune_system_;

  // The starting drug values given for a complex therapy
  std::map<int, double> starting_mac_drug_values;

  // Get the number of days that the individual complied with the therapy
  static int complied_dosing_days(const SCTherapy* therapy) ;

 public:
  Person();
  ~Person() override;

  // Check to see if the indicated event has been defined for the individual.
  template <typename T>
  bool has_event() const {
    for (Event *e : *events()) {
      if (dynamic_cast<T *>(e) != nullptr && e->executable && e->dispatcher != nullptr) {
        return true;
      }
    }
    return false;
  }

  ul_uid get_uid() const { return _uid; }

  ImmuneSystem* immune_system() const { return immune_system_; }

  void init() override;

  void NotifyChange(const Property &property, const void *oldValue, const void *newValue);

  virtual void increase_age_by_1_year();

  ClonalParasitePopulation *add_new_parasite_to_blood(Genotype *parasite_type) const;

  // Notify the individual of the change to the force of infection (FOI) for one of their parasite clones.
  //
  // Sign - positive to increment the global FOI, negative to decrement the global FOI
  // blood_parasite_log_relative_density - The parasite density for a single parasite clone in the individual.
  // log_total_relative_parasite_density - The total parasite density for all clones present in the individual
  virtual void notify_change_in_force_of_infection(const double &sign, const int &parasite_type_id,
                                                   const double &blood_parasite_log_relative_density,
                                                   const double &log_total_relative_parasite_density);

  virtual double get_biting_level_value();

  // Calculate the infectivity of an arbitrary parasite population based upon the parasite density. This is based upon
  // the calculations by Ross et al. (2006)
  virtual double relative_infectivity(const double &log10_parasite_density);

  virtual double get_probability_progress_to_clinical();

  virtual bool will_progress_to_death_when_receive_no_treatment();

  virtual bool will_progress_to_death_when_receive_treatment();

  void cancel_all_other_progress_to_clinical_events_except(Event *event) const;

  void cancel_all_events_except(Event *event) const;

  void change_all_parasite_update_function(ParasiteDensityUpdateFunction *from,
                                           ParasiteDensityUpdateFunction *to) const;

  void receive_therapy(Therapy *therapy, ClonalParasitePopulation *clinical_caused_parasite, bool is_mac_therapy = false);

  void add_drug_to_blood(DrugType *dt, const int &dosing_days, bool is_mac_therapy);

  void schedule_progress_to_clinical_event_by(ClonalParasitePopulation *blood_parasite);

  void schedule_test_treatment_failure_event(ClonalParasitePopulation *blood_parasite, const int &testing_day,
                                             const int &t_id = 0);

  void schedule_update_by_drug_event(ClonalParasitePopulation *clinical_caused_parasite);

  void schedule_end_clinical_event(ClonalParasitePopulation *clinical_caused_parasite);

  void schedule_end_clinical_by_no_treatment_event(ClonalParasitePopulation *clinical_caused_parasite);

  void schedule_relapse_event(ClonalParasitePopulation *clinical_caused_parasite, const int &time_until_relapse);

  void schedule_move_parasite_to_blood(Genotype *genotype, const int &time);

  void schedule_mature_gametocyte_event(ClonalParasitePopulation *clinical_caused_parasite);

  void schedule_update_every_K_days_event(const int &time);

  void change_state_when_no_parasite_in_blood();

  void determine_relapse_or_not(ClonalParasitePopulation *clinical_caused_parasite);

  void determine_clinical_or_not(ClonalParasitePopulation *clinical_caused_parasite);

  void update() override;

  void update_current_state();

  void randomly_choose_parasite();

  void infected_by(const int &parasite_type_id);

  bool inflict_bite(unsigned int parasite_type_id);

  void randomly_choose_target_location();

  void schedule_move_to_target_location_next_day_event(const int &location);

  bool has_return_to_residence_event() const;

  void cancel_all_return_to_residence_events() const;

  bool has_detectable_parasite() const;

  void increase_number_of_times_bitten();

  double get_age_dependent_biting_factor() const;

  void update_biting_level();

  bool isGametocytaemic() const;

  void generate_prob_present_at_mda_by_age();

  double prob_present_at_mda();

  bool has_effective_drug_in_blood() const;

  void receive_therapy(SCTherapy *sc_therapy, bool is_mac_therapy);

};

#endif
