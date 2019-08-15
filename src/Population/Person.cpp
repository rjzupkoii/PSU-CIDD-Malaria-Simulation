/* 
 * File:   Person.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:25 PM
 */

#include "Person.h"
#include "Population.h"
#include "ImmuneSystem.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Scheduler.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Events/ProgressToClinicalEvent.h"
#include "ClonalParasitePopulation.h"
#include "Events/EndClinicalDueToDrugResistanceEvent.h"
#include "Events/TestTreatmentFailureEvent.h"
#include "Therapies/SCTherapy.h"
#include "Therapies/DrugType.h"
#include "Events/UpdateWhenDrugIsPresentEvent.h"
#include "Events/EndClinicalEvent.h"
#include "Events/EndClinicalByNoTreatmentEvent.h"
#include "DrugsInBlood.h"
#include "Therapies/Drug.h"
#include "Core/Random.h"
#include "Events/MoveParasiteToBloodEvent.h"
#include "Events/MatureGametocyteEvent.h"
#include "Events/UpdateEveryKDaysEvent.h"
#include "Events/ReturnToResidenceEvent.h"
#include "Events/CirculateToTargetLocationNextDayEvent.h"
#include "MDC/ModelDataCollector.h"
#include "Events/BirthdayEvent.h"
#include "Therapies/MACTherapy.h"
#include "Events/ReceiveTherapyEvent.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>
#include "Helpers/ObjectHelpers.h"

OBJECTPOOL_IMPL(Person)

Person::Person() :
  location_(-1), residence_location_(-1), host_state_(SUSCEPTIBLE), age_(-1), age_class_(-1), birthday_(-1),
  latest_update_time_(-1), bitting_level_(-1), base_bitting_level_value_(0), moving_level_(-1),
  liver_parasite_type_(nullptr),
  number_of_times_bitten_(0),
  number_of_trips_taken_(0),
  last_therapy_id_(0) {
  population_ = nullptr;
  immune_system_ = nullptr;
  all_clonal_parasite_populations_ = nullptr;
  drugs_in_blood_ = nullptr;

  today_infections_ = nullptr;
  today_target_locations_ = nullptr;
  latest_update_time_ = -1;

}

void Person::init() {
  Dispatcher::init();

  immune_system_ = new ImmuneSystem(this);

  all_clonal_parasite_populations_ = new SingleHostClonalParasitePopulations(this);
  all_clonal_parasite_populations_->init();

  drugs_in_blood_ = new DrugsInBlood(this);
  drugs_in_blood_->init();

  today_infections_ = new IntVector();
  today_target_locations_ = new IntVector();
}

Person::~Person() {
  Dispatcher::clear_events();
  ObjectHelpers::delete_pointer<ImmuneSystem>(immune_system_);
  ObjectHelpers::delete_pointer<SingleHostClonalParasitePopulations>(all_clonal_parasite_populations_);
  ObjectHelpers::delete_pointer<DrugsInBlood>(drugs_in_blood_);
  ObjectHelpers::delete_pointer<IntVector>(today_infections_);
  ObjectHelpers::delete_pointer<IntVector>(today_target_locations_);
}

void Person::NotifyChange(const Property &property, const void* oldValue, const void* newValue) {
  if (population_ != nullptr) {
    population_->notify_change(this, property, oldValue, newValue);
  }
}

int Person::location() const {
  return location_;
}

void Person::set_location(const int &value) {
  if (location_ != value) {
    all_clonal_parasite_populations_->remove_all_infection_force();
    if (Model::DATA_COLLECTOR != nullptr) {
      const auto day_diff = (Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year());
      if (location_ != -1) {
        Model::DATA_COLLECTOR->update_person_days_by_years(location_, -day_diff);
      }
      Model::DATA_COLLECTOR->update_person_days_by_years(value, day_diff);
    }

    Model::DATA_COLLECTOR->record_1_migration(this, location_, value);

    NotifyChange(LOCATION, &location_, &value);

    location_ = value;
    all_clonal_parasite_populations_->add_all_infection_force();
  }
}

Person::HostStates Person::host_state() const {
  return host_state_;
}

void Person::set_host_state(const HostStates &value) {
  if (host_state_ != value) {
    NotifyChange(HOST_STATE, &host_state_, &value);
    if (value == DEAD) {
      //clear also remove all infection forces
      all_clonal_parasite_populations_->clear();
      clear_events();

      //
      //            Model::STATISTIC->update_person_days_by_years(location_, -(Constants::DAYS_IN_YEAR() - Model::SCHEDULER->current_day_in_year()));
      Model::DATA_COLLECTOR->record_1_death(location_, birthday_, number_of_times_bitten_, age_class_, age_);
    }

    host_state_ = value;
  }

}

int Person::age() const {
  return age_;
}

void Person::set_age(const int &value) {
  if (age_ != value) {
    //TODO::if age access the limit of age structure i.e. 100, remove person???

    NotifyChange(AGE, &age_, &value);

    // update bitting rate level
    age_ = value;

    //update age class
    if (Model::MODEL != nullptr) {
      auto ac = age_class_ == -1 ? 0 : age_class_;

      while (ac < (Model::CONFIG->number_of_age_classes() - 1) && age_ >= Model::CONFIG->age_structure()[ac]) {
        ac++;
      }

      set_age_class(ac);
    }
  }
}

int Person::age_class() const {
  return age_class_;
}

void Person::set_age_class(const int &value) {
  if (age_class_ != value) {
    NotifyChange(AGE_CLASS, &age_class_, &value);
    age_class_ = value;
  }
}

int Person::bitting_level() const {
  return bitting_level_;
}

void Person::set_bitting_level(const int &value) {
  auto new_value = value;
  if (new_value < 0) {
    new_value = 0;
  }

  if (new_value > (Model::CONFIG->relative_bitting_info().number_of_biting_levels - 1)) {
    new_value = Model::CONFIG->relative_bitting_info().number_of_biting_levels - 1;
  }
  if (bitting_level_ != new_value) {
    all_clonal_parasite_populations_->remove_all_infection_force();

    NotifyChange(BITTING_LEVEL, &bitting_level_, &new_value);
    bitting_level_ = new_value;
    all_clonal_parasite_populations_->add_all_infection_force();
  }
}

int Person::moving_level() const {
  return moving_level_;
}

void Person::set_moving_level(const int &value) {
  if (moving_level_ != value) {
    NotifyChange(MOVING_LEVEL, &moving_level_, &value);
    moving_level_ = value;
  }
}

void Person::increase_age_by_1_year() {
  set_age(age_ + 1);
}

ImmuneSystem* Person::immune_system() const {
  return immune_system_;
}

void Person::set_immune_system(ImmuneSystem* value) {
  if (immune_system_ != value) {
    delete immune_system_;
    immune_system_ = value;
  }
}

ClonalParasitePopulation* Person::add_new_parasite_to_blood(Genotype* parasite_type) const {
  auto* blood_parasite = new ClonalParasitePopulation(parasite_type);

  all_clonal_parasite_populations_->add(blood_parasite);

  blood_parasite->set_last_update_log10_parasite_density(
    Model::CONFIG->parasite_density_level().log_parasite_density_from_liver);

  return blood_parasite;
}

void Person::notify_change_in_force_of_infection(const double &sign, const int &parasite_type_id,
                                                 const double &blood_parasite_log_relative_density,
                                                 const double &log_total_relative_parasite_density) {
  if (blood_parasite_log_relative_density == 0.0) {

    return;
  }

  //    double weight = pow(10, blood_parasite_log_relative_density - log_total_relative_parasite_density);
  //    assert(weight <=1 && weight >=0);
  const auto relative_force_of_infection =
    sign * get_biting_level_value() * relative_infectivity(log_total_relative_parasite_density) *
    blood_parasite_log_relative_density;

  population_->notify_change_in_force_of_infection(location_, parasite_type_id, relative_force_of_infection);
}

double Person::get_biting_level_value() {

  return Model::CONFIG->relative_bitting_info().v_biting_level_value[bitting_level_];
}

double Person::relative_infectivity(const double &log10_parasite_density) {
  const auto d_n = log10_parasite_density * Model::CONFIG->relative_infectivity().sigma +
                   Model::CONFIG->relative_infectivity().ro_star;
  const auto p = Model::RANDOM->cdf_standard_normal_distribution(d_n);

  return p * p + 0.01;
}

double Person::get_probability_progress_to_clinical() {
  return immune_system_->get_clinical_progression_probability();
}

void Person::cancel_all_other_progress_to_clinical_events_except(Event* event) const {
  for (auto* e : *events()) {
    if (e != event && dynamic_cast<ProgressToClinicalEvent*>(e) != nullptr) {
      //            std::cout << "Hello"<< std::endl;
      e->executable = false;
    }
  }
}

void Person::cancel_all_events_except(Event* event) const {

  for (auto* e : *events()) {
    if (e != event) {
      //            e->set_dispatcher(nullptr);
      e->executable = false;
    }
  }
}

//void Person::record_treatment_failure_for_test_treatment_failure_events() {
//
//    for(Event* e :  *events()) {
//        if (dynamic_cast<TestTreatmentFailureEvent*> (e) != nullptr && e->executable()) {
//            //            e->set_dispatcher(nullptr);
//            //record treatment failure
//            Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(location_, age_, ((TestTreatmentFailureEvent*) e)->therapyId());
//
//        }
//    }
//}

void
Person::change_all_parasite_update_function(ParasiteDensityUpdateFunction* from,
                                            ParasiteDensityUpdateFunction* to) const {
  all_clonal_parasite_populations_->change_all_parasite_update_function(from, to);
}

bool Person::will_progress_to_death_when_receive_no_treatment() {
  //yes == death
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);
  return p <= Model::CONFIG->mortality_when_treatment_fail_by_age_class()[age_class_];
}

bool Person::will_progress_to_death_when_recieve_treatment() {
  //yes == death
  double P = Model::RANDOM->random_flat(0.0, 1.0);
  // 90% lower than no treatment
  return P <= Model::CONFIG->mortality_when_treatment_fail_by_age_class()[age_class_] * (1 - 0.9);
}

void Person::schedule_progress_to_clinical_event_by(ClonalParasitePopulation* blood_parasite) {

  const auto time = (age_ <= 5)
                    ? Model::CONFIG->days_to_clinical_under_five()
                    : Model::CONFIG->days_to_clinical_over_five();

  ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, this, blood_parasite,
                                          Model::SCHEDULER->current_time() + time);
}

void Person::schedule_end_clinical_due_to_drug_resistance_event(ClonalParasitePopulation* blood_parasite) {

  auto d_clinical = Model::RANDOM->random_normal(7, 2);
  d_clinical = std::min<int>(std::max<int>(d_clinical, 5), 14);

  EndClinicalDueToDrugResistanceEvent::schedule_event(Model::SCHEDULER, this, blood_parasite,
                                                      Model::SCHEDULER->current_time() + d_clinical);
}

void Person::schedule_test_treatment_failure_event(ClonalParasitePopulation* blood_parasite, const int &testing_day,
                                                   const int &t_id) {
  TestTreatmentFailureEvent::schedule_event(Model::SCHEDULER, this, blood_parasite,
                                            Model::SCHEDULER->current_time() + testing_day, t_id);
}

int Person::complied_dosing_days(const int &dosing_day) const {

  if (Model::CONFIG->p_compliance() < 1) {
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);
    if (p > Model::CONFIG->p_compliance()) {
      //do not comply
      const auto a = (Model::CONFIG->min_dosing_days() - dosing_day) / (1 - Model::CONFIG->p_compliance());
      return static_cast<int>(std::ceil(a * p + Model::CONFIG->min_dosing_days() - a));
    }
  }
  return dosing_day;
}

void Person::receive_therapy(Therapy* therapy, ClonalParasitePopulation* clinical_caused_parasite) {
  //if therapy is SCTherapy
  auto* sc_therapy = dynamic_cast<SCTherapy*>(therapy);
  if (sc_therapy != nullptr) {

    for (int j = 0; j < sc_therapy->drug_ids.size(); ++j) {
      int drug_id = sc_therapy->drug_ids[j];
      auto dosing_days = sc_therapy->drug_ids.size() == sc_therapy->dosing_day.size() ? sc_therapy->dosing_day[j]
                                                                                      : sc_therapy->dosing_day[0];

      dosing_days = complied_dosing_days(dosing_days);
//      std::cout << drug_id << "-" << dosing_days << std::endl;

      add_drug_to_blood(Model::CONFIG->drug_db()->at(drug_id), dosing_days);
    }

    for (auto drug_id : sc_therapy->drug_ids) {

    }
  } else {
    //else if therapy is MACTherapy
    auto* mac_therapy = dynamic_cast<MACTherapy*>(therapy);
    assert(mac_therapy != nullptr);
    for (auto i = 0; i < mac_therapy->therapy_ids().size(); i++) {
      const auto therapy_id = mac_therapy->therapy_ids()[i];
      const auto start_day = mac_therapy->start_at_days()[i];

      if (start_day == 1) {
        receive_therapy(Model::CONFIG->therapy_db()[therapy_id], clinical_caused_parasite);
      } else {
        assert(start_day > 1);
        ReceiveTherapyEvent::schedule_event(Model::SCHEDULER, this, Model::CONFIG->therapy_db()[therapy_id],
                                            Model::SCHEDULER->current_time() + start_day - 1,
                                            clinical_caused_parasite);
      }
    }
  }

  last_therapy_id_ = therapy->id();
}

void Person::add_drug_to_blood(DrugType* dt, const int &dosing_days) {
  auto* drug = new Drug(dt);
  drug->set_dosing_days(dosing_days);
  drug->set_last_update_time(Model::SCHEDULER->current_time());

  const auto sd = dt->age_group_specific_drug_concentration_sd()[age_class_];
  //    std::cout << ageClass << "====" << sd << std::endl;
  const auto drug_level = Model::RANDOM->random_normal_truncated(1.0, sd);

  drug->set_last_update_value(drug_level);
  drug->set_starting_value(drug_level);

  drug->set_start_time(Model::SCHEDULER->current_time());
  drug->set_end_time(Model::SCHEDULER->current_time() + dt->get_total_duration_of_drug_activity(dosing_days));

  drugs_in_blood_->add_drug(drug);

}

void Person::schedule_update_by_drug_event(ClonalParasitePopulation* clinical_caused_parasite) {

  UpdateWhenDrugIsPresentEvent::schedule_event(Model::SCHEDULER, this, clinical_caused_parasite,
                                               Model::SCHEDULER->current_time() + 1);
}

void Person::schedule_end_clinical_event(ClonalParasitePopulation* clinical_caused_parasite) {

  int dClinical = Model::RANDOM->random_normal(7, 2);
  dClinical = std::min<int>(std::max<int>(dClinical, 5), 14);

  EndClinicalEvent::schedule_event(Model::SCHEDULER, this, clinical_caused_parasite,
                                   Model::SCHEDULER->current_time() + dClinical);
}

void Person::schedule_end_clinical_by_no_treatment_event(ClonalParasitePopulation* clinical_caused_parasite) {

  auto d_clinical = Model::RANDOM->random_normal(7, 2);
  d_clinical = std::min<int>(std::max<int>(d_clinical, 5), 14);

  EndClinicalByNoTreatmentEvent::schedule_event(Model::SCHEDULER, this, clinical_caused_parasite,
                                                Model::SCHEDULER->current_time() + d_clinical);

}

void Person::change_state_when_no_parasite_in_blood() {
  if (all_clonal_parasite_populations_->size() == 0) {
    if (liver_parasite_type_ == nullptr) {
      //        std::cout << "S" << std::endl;
      //        std::cout << host_state_<< std::endl;
      set_host_state(SUSCEPTIBLE);
      //        std::cout << "ES" << std::endl;

    } else {

      set_host_state(EXPOSED);
    }
    immune_system_->set_increase(false);
  }
}

void Person::determine_relapse_or_not(ClonalParasitePopulation* clinical_caused_parasite) {
  if (all_clonal_parasite_populations_->contain(clinical_caused_parasite)) {
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);

    if (p <= Model::CONFIG->p_relapse()) {
      //        if (P <= get_probability_progress_to_clinical()) {
      //progress to clinical after several days
      clinical_caused_parasite->set_update_function(Model::MODEL->progress_to_clinical_update_function());
      clinical_caused_parasite->set_last_update_log10_parasite_density(
        Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic);
      schedule_relapse_event(clinical_caused_parasite, Model::CONFIG->relapse_duration());

    } else {
      //progress to clearance
      if (clinical_caused_parasite->last_update_log10_parasite_density() >
          Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic) {
        clinical_caused_parasite->set_last_update_log10_parasite_density(
          Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic);
      }
      clinical_caused_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());

    }
  }
}

void Person::determine_clinical_or_not(ClonalParasitePopulation* clinical_caused_parasite) {
  if (all_clonal_parasite_populations_->contain(clinical_caused_parasite)) {
    const auto p = Model::RANDOM->random_flat(0.0, 1.0);

    //        if (P <= Model::CONFIG->p_relapse()) {
//    if (Model::SCHEDULER->current_time() >= 2000 && Model::SCHEDULER->current_time() <= 2010)
//      std::cout << this->age() << "\t" << this->immune_system()->get_current_value() << "\t"
//                << get_probability_progress_to_clinical()
//                << std::endl;
    if (p <= get_probability_progress_to_clinical()) {
      //progress to clinical after several days
      clinical_caused_parasite->set_update_function(Model::MODEL->progress_to_clinical_update_function());
      clinical_caused_parasite->set_last_update_log10_parasite_density(
        Model::CONFIG->parasite_density_level().log_parasite_density_asymptomatic);
      schedule_relapse_event(clinical_caused_parasite, Model::CONFIG->relapse_duration());

    } else {
      //progress to clearance

      clinical_caused_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());

    }
  }
}

void Person::schedule_relapse_event(ClonalParasitePopulation* clinical_caused_parasite, const int &time_until_relapse) {

  int duration = Model::RANDOM->random_normal(time_until_relapse, 15);
  duration = std::min<int>(std::max<int>(duration, time_until_relapse - 15), time_until_relapse + 15);
  ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, this, clinical_caused_parasite,
                                          Model::SCHEDULER->current_time() + duration);
}

void Person::update() {
  //    std::cout << "Person Update"<< std::endl;
  //already update
  assert(host_state_ != DEAD);

  if (latest_update_time_ == Model::SCHEDULER->current_time()) return;

  //    std::cout << "ppu"<< std::endl;
  ///update the density of each blood parasite in parasite population
  // parasite will be kill by immune system
  all_clonal_parasite_populations_->update();

  //    std::cout << "dibu"<< std::endl;
  //update all drugs concentration
  drugs_in_blood_->update();

  //update drug activity on parasite
  all_clonal_parasite_populations_->update_by_drugs(drugs_in_blood_);

  //    std::cout << "imm"<< std::endl;
  immune_system_->update();

  //    std::cout << "csu"<< std::endl;
  update_current_state();

  //    Dispatcher::update();

  //update bitting level only less than 1 to save performance
  // the other will be update in birthday event
  update_bitting_level();

  latest_update_time_ = Model::SCHEDULER->current_time();
  //    std::cout << "End Person Update"<< std::endl;
}

void Person::update_bitting_level() {
  if (Model::CONFIG->using_age_dependent_bitting_level()) {

    //TODO: test here
    const auto new_bitting_level_value = base_bitting_level_value_ * get_age_dependent_biting_factor();
    const auto diff_in_level = static_cast<int>(std::floor(new_bitting_level_value - get_biting_level_value()) /
                                                ((Model::CONFIG->relative_bitting_info().max_relative_biting_value -
                                                  1) /
                                                 static_cast<double>(
                                                   Model::CONFIG->relative_bitting_info().number_of_biting_levels -
                                                   1)));
    if (diff_in_level != 0) {
      //            std::cout << bitting_level_ << "\t" << diff_in_level << std::endl;
      set_bitting_level(bitting_level_ + diff_in_level);
      //              std::cout << "ok" << std::endl;
    }
  }
}

void Person::update_current_state() {
  //    std::cout << "ccod" << std::endl;
  //clear drugs <=0.1
  drugs_in_blood_->clear_cut_off_drugs_by_event(nullptr);
  //    std::cout << "ccp" << std::endl;
  //clear cured parasite
  all_clonal_parasite_populations_->clear_cured_parasites();

  //    std::cout << "change state" << std::endl;
  if (all_clonal_parasite_populations_->size() == 0) {
    change_state_when_no_parasite_in_blood();
  } else {

    immune_system_->set_increase(true);
  }

}

void Person::randomly_choose_parasite() {
  if (today_infections_->empty()) {
    //already chose
    return;
  }
  if (today_infections_->size() == 1) {
    infected_by(today_infections_->at(0));
  } else {
    const std::size_t index_random_parasite = Model::RANDOM->random_uniform(today_infections_->size());
    infected_by(today_infections_->at(index_random_parasite));
  }

  today_infections_->clear();
}

void Person::infected_by(const int &parasite_type_id) {
  //only infect if liver is available :D
  if (liver_parasite_type_ == nullptr) {
    if (host_state_ == SUSCEPTIBLE) {
      set_host_state(EXPOSED);
    }

    Genotype* genotype = Model::CONFIG->genotype_db()->at(parasite_type_id);
    liver_parasite_type_ = genotype;

    //move parasite to blood in next 7 days
    schedule_move_parasite_to_blood(genotype, 7);
  }

}

void Person::schedule_move_parasite_to_blood(Genotype* genotype, const int &time) {

  MoveParasiteToBloodEvent::schedule_event(Model::SCHEDULER, this, genotype, Model::SCHEDULER->current_time() + time);
}

void Person::schedule_mature_gametocyte_event(ClonalParasitePopulation* clinical_caused_parasite) {
  const auto day_mature_gametocyte = (age_ <= 5)
                                     ? Model::CONFIG->days_mature_gametocyte_under_five()
                                     : Model::CONFIG->days_mature_gametocyte_over_five();
  MatureGametocyteEvent::schedule_event(Model::SCHEDULER, this, clinical_caused_parasite,
                                        Model::SCHEDULER->current_time() + day_mature_gametocyte);
}

void Person::schedule_update_every_K_days_event(const int &time) {
  UpdateEveryKDaysEvent::schedule_event(Model::SCHEDULER, this, Model::SCHEDULER->current_time() + time);
}

void Person::randomly_choose_target_location() {
  if (today_target_locations_->empty()) {
    //already chose
    return;
  }

  auto target_location{-1};
  if (today_target_locations_->size() == 1) {
    target_location = today_target_locations_->at(0);
  } else {
    const int index_random_location = Model::RANDOM->random_uniform(today_target_locations_->size());
    target_location = today_target_locations_->at(index_random_location);
  }

  schedule_move_to_target_location_next_day_event(target_location);

  today_target_locations_->clear();
}

void Person::schedule_move_to_target_location_next_day_event(const int &location) {
  this->number_of_trips_taken_ += 1;
  CirculateToTargetLocationNextDayEvent::schedule_event(Model::SCHEDULER, this, location,
                                                        Model::SCHEDULER->current_time() + 1);
}

bool Person::has_return_to_residence_event() const {

  for (Event* e : *events()) {
    if (dynamic_cast<ReturnToResidenceEvent*>(e) != nullptr) {
      return true;
    }
  }
  return false;
}

void Person::cancel_all_return_to_residence_events() const {

  for (Event* e : *events()) {
    if (dynamic_cast<ReturnToResidenceEvent*>(e) != nullptr) {
      e->executable = false;
    }
  }
}

bool Person::has_detectable_parasite() const {

  return all_clonal_parasite_populations_->has_detectable_parasite();
}

void Person::increase_number_of_times_bitten() {
  if (Model::SCHEDULER->current_time() >= Model::CONFIG->start_collect_data_day()) {
    number_of_times_bitten_++;
  }
}

void Person::move_to_population(Population* target_population) {
  assert(population_ != target_population);

  population_->remove_person(this);
  target_population->add_person(this);
}

bool Person::has_birthday_event() const {

  for (Event* e : *events()) {
    if (dynamic_cast<BirthdayEvent*>(e) != nullptr) {
      return true;
    }
  }
  return false;
}

bool Person::has_update_by_having_drug_event() const {

  for (Event* e : *events()) {
    if (dynamic_cast<UpdateWhenDrugIsPresentEvent*>(e) != nullptr) {
      return true;
    }
  }
  return false;
}

double Person::get_age_dependent_biting_factor() const {
  //
  //0.00 - 0.25  -  6.5
  //0.25 - 0.50  -  8.0
  //0.50 - 0.75  -  9.0
  //0.75 - 1.00  -  9.5
  //1.00 - 2.00  -  11.0
  //2.00 - 3.00  -  13.5
  //3.00 - 4.00  -  15.5
  //4.00 - 5.00  -  17.5
  // + 2.75kg until 20
  // then divide by 61.5

  if (age_ < 1) {
    const auto age = ((Model::SCHEDULER->current_time() - birthday_) % Constants::DAYS_IN_YEAR()) /
                     static_cast<double>(Constants::DAYS_IN_YEAR());
    if (age < 0.25)
      return 0.106;
    if (age < 0.5)
      return 0.13;
    if (age < 0.75)
      return 0.1463;
    return 0.1545;
  }
  if (age_ < 2)
    return 0.1789;
  if (age_ < 3)
    return 0.2195;
  if (age_ < 4)
    return 0.2520;
  if (age_ < 20)
    return (17.5 + (age_ - 4) * 2.75) / 61.5;
  return 1.0;
}

double Person::p_infection_from_an_infectious_bite() const {
  return (1 - immune_system_->get_current_value()) / 8.333 + 0.04;
}

bool Person::isGametocytaemic() const {
  return all_clonal_parasite_populations_->is_gametocytaemic();
}

void Person::generate_prob_present_at_mda_by_age() {
  if (prob_present_at_mda_by_age().empty()) {
    for (auto i = 0; i < Model::CONFIG->mean_prob_individual_present_at_mda().size(); i++) {
      auto value = Model::RANDOM->random_beta(
        Model::CONFIG->prob_individual_present_at_mda_distribution()[i].alpha,
        Model::CONFIG->prob_individual_present_at_mda_distribution()[i].beta);
      prob_present_at_mda_by_age_.push_back(value);
    }
  }
}

double Person::prob_present_at_mda() {
  auto i = 0;
  // std::cout << "hello " << i << std::endl;
  while (age_ > Model::CONFIG->age_bracket_prob_individual_present_at_mda()[i]
         && i < Model::CONFIG->age_bracket_prob_individual_present_at_mda().size()) {
    i++;
  }

  return prob_present_at_mda_by_age_[i];
}

bool Person::has_effective_drug_in_blood() const {
  for (const auto &kv_drug : *drugs_in_blood_->drugs()) {
    if (kv_drug.second->last_update_value() > 0.5) return true;
  }
  return false;
}
