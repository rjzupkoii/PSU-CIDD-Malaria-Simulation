/* 
 * File:   Model.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:26 PM
 */
#include <fmt/format.h>
#include "Model.h"
#include "Population/Population.h"
#include "Core/Config/Config.h"
#include "Population/Person.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"
#include "Events/BirthdayEvent.h"
#include "Events/ProgressToClinicalEvent.h"
#include "Events/EndClinicalDueToDrugResistanceEvent.h"
#include "Events/TestTreatmentFailureEvent.h"
#include "Events/UpdateWhenDrugIsPresentEvent.h"
#include "Events/EndClinicalByNoTreatmentEvent.h"
#include "Events/EndClinicalEvent.h"
#include "Therapies/Drug.h"
#include "Population/ImmuneSystem.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Events/MatureGametocyteEvent.h"
#include "Events/MoveParasiteToBloodEvent.h"
#include "Events/UpdateEveryKDaysEvent.h"
#include "Reporters/Reporter.h"
#include "Events/CirculateToTargetLocationNextDayEvent.h"
#include "Events/ReturnToResidenceEvent.h"
#include "Population/ClonalParasitePopulation.h"
#include "Events/SwitchImmuneComponentEvent.h"
#include "Events/Population/ImportationPeriodicallyEvent.h"
#include "Events/Population/ImportationEvent.h"
#include "easylogging++.h"
#include "Helpers/ObjectHelpers.h"
#include "Strategies/IStrategy.h"
#include "Malaria/SteadyTCM.h"
#include "Constants.h"
#include "Helpers/TimeHelpers.h"

Model* Model::MODEL = nullptr;
Config* Model::CONFIG = nullptr;
Random* Model::RANDOM = nullptr;
Scheduler* Model::SCHEDULER = nullptr;
ModelDataCollector* Model::DATA_COLLECTOR = nullptr;
Population* Model::POPULATION = nullptr;
IStrategy* Model::TREATMENT_STRATEGY = nullptr;
ITreatmentCoverageModel* Model::TREATMENT_COVERAGE = nullptr;
// std::shared_ptr<spdlog::logger> LOGGER;

Model::Model(const int &object_pool_size) {
  initialize_object_pool(object_pool_size);
  random_ = new Random();
  config_ = new Config(this);
  scheduler_ = new Scheduler(this);
  population_ = new Population(this);
  data_collector_ = new ModelDataCollector(this);

  MODEL = this;
  CONFIG = config_;
  SCHEDULER = scheduler_;
  RANDOM = random_;
  DATA_COLLECTOR = data_collector_;
  POPULATION = population_;

  // LOGGER = spdlog::stdout_logger_mt("console");

  progress_to_clinical_update_function_ = new ClinicalUpdateFunction(this);
  immunity_clearance_update_function_ = new ImmunityClearanceUpdateFunction(this);
  having_drug_update_function_ = new ImmunityClearanceUpdateFunction(this);
  clinical_update_function_ = new ImmunityClearanceUpdateFunction(this);

  reporters_ = std::vector<Reporter*>();

  initial_seed_number_ = 0;
  config_filename_ = "config.yml";
  tme_filename_ = "tme.txt";
  override_parameter_filename_ = "";
  override_parameter_line_number_ = -1;
  gui_type_ = -1;
  is_farm_output_ = false;
  cluster_job_number_ = 0;
  reporter_type_ = "";
}

Model::~Model() {
  release();

  release_object_pool();
}

void Model::set_treatment_strategy(const int &strategy_id) {
  treatment_strategy_ = strategy_id == -1 ? nullptr : config_->strategy_db()[strategy_id];
  TREATMENT_STRATEGY = treatment_strategy_;

  treatment_strategy_->adjust_started_time_point(Model::SCHEDULER->current_time());

  //
  // if (treatment_strategy_->get_type() == IStrategy::NestedSwitching) {
  //   dynamic_cast<NestedSwitchingStrategy *>(treatment_strategy_)->initialize_update_time(config_);
  // }
  // if (treatment_strategy_->get_type() == IStrategy::NestedSwitchingDifferentDistributionByLocation) {
  //   dynamic_cast<NestedMFTMultiLocationStrategy *>(treatment_strategy_)->initialize_update_time(config_);
  // }
}

void Model::set_treatment_coverage(ITreatmentCoverageModel* tcm) {
  if (treatment_coverage_ != tcm) {
    if (tcm->p_treatment_less_than_5.empty() || tcm->p_treatment_more_than_5.empty()) {
      //copy current value
      tcm->p_treatment_less_than_5 = treatment_coverage_->p_treatment_less_than_5;
      tcm->p_treatment_more_than_5 = treatment_coverage_->p_treatment_more_than_5;
    }

    ObjectHelpers::delete_pointer<ITreatmentCoverageModel>(treatment_coverage_);
  }
  treatment_coverage_ = tcm;
  TREATMENT_COVERAGE = tcm;
}

void Model::build_initial_treatment_coverage() {
  auto* tcm = new SteadyTCM();
  for (auto &location : config_->location_db()) {
    tcm->p_treatment_less_than_5.push_back(location.p_treatment_less_than_5);
    tcm->p_treatment_more_than_5.push_back(location.p_treatment_more_than_5);
  }
  set_treatment_coverage(tcm);
}

void Model::initialize() {
  LOG(INFO) << "Model initilizing...";

  LOG(INFO) << "Initialize Random";
  //Initialize Random Seed
  random_->initialize(initial_seed_number_);

  LOG(INFO) << fmt::format("Read input file: {}", config_filename_);
  //Read input file
  config_->read_from_file(config_filename_);

  //add reporter here
  if (reporter_type_.empty()) {
    add_reporter(Reporter::MakeReport(Reporter::MONTHLY_REPORTER));
  } else {
    if (Reporter::ReportTypeMap.find(reporter_type_) != Reporter::ReportTypeMap.end()) {
      add_reporter(Reporter::MakeReport(Reporter::ReportTypeMap[reporter_type_]));
    }
  }

  LOG(INFO) << "Initialing reports";
  //initialize reporters
  for (auto* reporter : reporters_) {
    reporter->initialize();
  }

  LOG(INFO) << "Initialzing scheduler";
  LOG(INFO) << "Starting day is " << CONFIG->starting_date();
  //initialize scheduler
  scheduler_->initialize(CONFIG->starting_date(), config_->total_time());

  LOG(INFO) << "Initialing initial strategy";
  //set treatment strategy
  set_treatment_strategy(config_->initial_strategy_id());

  LOG(INFO) << "Initialing initial treatment coverage model";
  build_initial_treatment_coverage();

  LOG(INFO) << "Initializing data collector";
  //initialize data_collector
  data_collector_->initialize();

  LOG(INFO) << "Initializing population";
  //initialize Population
  population_->initialize();

  LOG(INFO) << "Introducing initial cases";
  //initialize infected_cases
  population_->introduce_initial_cases();

  //initialize external population
  //    external_population_->initialize();

  LOG(INFO) << "Schedule for population event";
  for (auto* event : config_->preconfig_population_events()) {
    scheduler_->schedule_population_event(event);
//    LOG(INFO) << scheduler_->population_events_list_[event->time].size();
  }
  //
  // for(auto it = CONFIG->genotype_db()->begin(); it != CONFIG->genotype_db()->end(); ++it) {
  //   std::cout << it->first << " : " << it->second->daily_fitness_multiple_infection() << std::endl;
  // }
}

void Model::initialize_object_pool(const int &size) {
  BirthdayEvent::InitializeObjectPool(size);
  ProgressToClinicalEvent::InitializeObjectPool(size);
  EndClinicalDueToDrugResistanceEvent::InitializeObjectPool(size);
  UpdateWhenDrugIsPresentEvent::InitializeObjectPool(size);
  EndClinicalEvent::InitializeObjectPool(size);
  EndClinicalByNoTreatmentEvent::InitializeObjectPool(size);
  MatureGametocyteEvent::InitializeObjectPool(size);
  MoveParasiteToBloodEvent::InitializeObjectPool(size);
  UpdateEveryKDaysEvent::InitializeObjectPool(size);
  CirculateToTargetLocationNextDayEvent::InitializeObjectPool(size);
  ReturnToResidenceEvent::InitializeObjectPool(size);
  SwitchImmuneComponentEvent::InitializeObjectPool(size);
  ImportationPeriodicallyEvent::InitializeObjectPool(size);
  ImportationEvent::InitializeObjectPool(size);
  TestTreatmentFailureEvent::InitializeObjectPool(size);

  ClonalParasitePopulation::InitializeObjectPool(size);
  SingleHostClonalParasitePopulations::InitializeObjectPool();

  Drug::InitializeObjectPool(size);
  DrugsInBlood::InitializeObjectPool(size);

  //    InfantImmuneComponent::InitializeObjectPool(size);
  //    NonInfantImmuneComponent::InitializeObjectPool(size);

  ImmuneSystem::InitializeObjectPool(size);
  Person::InitializeObjectPool(size);
}

void Model::release_object_pool() {
  //    std::cout << "Release object pool" << std::endl;
  Person::ReleaseObjectPool();
  ImmuneSystem::ReleaseObjectPool();

  // TODO: Investigate why?
  //    InfantImmuneComponent::ReleaseObjectPool();
  //    NonInfantImmuneComponent::ReleaseObjectPool();

  DrugsInBlood::ReleaseObjectPool();
  Drug::ReleaseObjectPool();

  SingleHostClonalParasitePopulations::ReleaseObjectPool();
  ClonalParasitePopulation::ReleaseObjectPool();

  TestTreatmentFailureEvent::ReleaseObjectPool();
  ImportationEvent::ReleaseObjectPool();
  ImportationPeriodicallyEvent::ReleaseObjectPool();
  SwitchImmuneComponentEvent::ReleaseObjectPool();
  ReturnToResidenceEvent::ReleaseObjectPool();
  CirculateToTargetLocationNextDayEvent::ReleaseObjectPool();
  UpdateEveryKDaysEvent::ReleaseObjectPool();
  MoveParasiteToBloodEvent::ReleaseObjectPool();
  MatureGametocyteEvent::ReleaseObjectPool();
  EndClinicalByNoTreatmentEvent::ReleaseObjectPool();
  EndClinicalEvent::ReleaseObjectPool();
  UpdateWhenDrugIsPresentEvent::ReleaseObjectPool();
  EndClinicalDueToDrugResistanceEvent::ReleaseObjectPool();
  ProgressToClinicalEvent::ReleaseObjectPool();
  BirthdayEvent::ReleaseObjectPool();
}

void Model::run() {
  LOG(INFO) << "Model starting...";
  before_run();
  scheduler_->run();
  after_run();
  LOG(INFO) << "Model finished.";
}

void Model::before_run() {
  LOG(INFO) << "Perform before run events";
  for (auto* reporter : reporters_) {
    reporter->before_run();
  }
}

void Model::after_run() {
  LOG(INFO) << "Perform after run events";

  data_collector_->update_after_run();

  for (auto* reporter : reporters_) {
    reporter->after_run();
  }
}

void Model::begin_time_step() {
  //reset daily variables
  data_collector_->begin_time_step();
  report_begin_of_time_step();
}

void Model::perform_population_events_daily() const {
  // TODO: turn on and off time for art mutation in the input file
  population_->perform_infection_event();
  population_->perform_birth_event();
  population_->perform_circulation_event();
}

void Model::daily_update(const int &current_time) {
  //for safety remove all dead by calling perform_death_event
  population_->perform_death_event();

  //update / calculate daily UTL
  data_collector_->end_of_time_step();

  //update force of infection
  population_->update_force_of_infection(current_time);

  //check to switch strategy
  treatment_strategy_->update_end_of_time_step();
}

void Model::monthly_update() {
  monthly_report();

  //reset monthly variables
  data_collector()->monthly_update();

  //
  treatment_strategy_->monthly_update();

  //update treatment coverage
  treatment_coverage_->monthly_update();

}

// ReSharper disable once CppMemberFunctionMayBeConst
void Model::yearly_update() {
  data_collector_->perform_yearly_update();
}

void Model::release() {
  //    std::cout << "Model Release" << std::endl;
  ObjectHelpers::delete_pointer<ClinicalUpdateFunction>(progress_to_clinical_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(immunity_clearance_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(having_drug_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(clinical_update_function_);

  ObjectHelpers::delete_pointer<Population>(population_);
  //   ObjectHelpers::DeletePointer<ExternalPopulation>(external_population_);
  ObjectHelpers::delete_pointer<Scheduler>(scheduler_);
  ObjectHelpers::delete_pointer<ModelDataCollector>(data_collector_);

  treatment_strategy_ = nullptr;
  ObjectHelpers::delete_pointer<ITreatmentCoverageModel>(treatment_coverage_);

  ObjectHelpers::delete_pointer<Config>(config_);
  ObjectHelpers::delete_pointer<Random>(random_);

  for (Reporter* reporter : reporters_) {
    ObjectHelpers::delete_pointer<Reporter>(reporter);
  }
  reporters_.clear();

  MODEL = nullptr;
  CONFIG = nullptr;
  SCHEDULER = nullptr;
  RANDOM = nullptr;
  DATA_COLLECTOR = nullptr;
  POPULATION = nullptr;
  TREATMENT_STRATEGY = nullptr;
  TREATMENT_COVERAGE = nullptr;
}

void Model::monthly_report() {
  data_collector_->perform_population_statistic();

  for (auto* reporter : reporters_) {
    reporter->monthly_report();
  }

}

void Model::report_begin_of_time_step() {
  for (auto* reporter : reporters_) {
    reporter->begin_time_step();
  }
}

void Model::add_reporter(Reporter* reporter) {
  reporters_.push_back(reporter);
  reporter->set_model(this);
}

double Model::get_seasonal_factor(const date::sys_days &today, const int &location) const {
  if (!Model::CONFIG->seasonal_info().enable) {
    return 1;
  }
  const auto day_of_year = TimeHelpers::day_of_year(today);
  const auto is_rainy_period = Model::CONFIG->seasonal_info().phi[location] < Constants::DAYS_IN_YEAR() / 2.0
                               ? day_of_year >= Model::CONFIG->seasonal_info().phi[location]
                                 && day_of_year <=
                                    Model::CONFIG->seasonal_info().phi[location] + Constants::DAYS_IN_YEAR() / 2.0
                               : day_of_year >= Model::CONFIG->seasonal_info().phi[location]
                                 || day_of_year <=
                                    Model::CONFIG->seasonal_info().phi[location] - Constants::DAYS_IN_YEAR() / 2.0;

  return (is_rainy_period)
         ? (Model::CONFIG->seasonal_info().A[location] - Model::CONFIG->seasonal_info().min_value[location]) *
           sin(Model::CONFIG->seasonal_info().B[location] * day_of_year +
               Model::CONFIG->seasonal_info().C[location]) +
           Model::CONFIG->seasonal_info().min_value[location]
         : Model::CONFIG->seasonal_info().min_value[location];
}
