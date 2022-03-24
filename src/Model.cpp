/* 
 * File:   Model.cpp
 * 
 * Main class for the individually based model. Initializes all the relevant
 * objects, passes control to the scheduler, and manages the tear-down.
 */
#include "Model.h"

#include <fmt/format.h>

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
#include "Helpers/StringHelpers.h"
#include "Strategies/IStrategy.h"
#include "Malaria/SteadyTCM.h"
#include "Validation/MovementValidation.h"
#include "Spatial/SpatialModel.hxx"

Model* Model::MODEL = nullptr;
Config* Model::CONFIG = nullptr;
Random* Model::RANDOM = nullptr;
Scheduler* Model::SCHEDULER = nullptr;
ModelDataCollector* Model::DATA_COLLECTOR = nullptr;
Population* Model::POPULATION = nullptr;
IStrategy* Model::TREATMENT_STRATEGY = nullptr;
ITreatmentCoverageModel* Model::TREATMENT_COVERAGE = nullptr;

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

  progress_to_clinical_update_function_ = new ClinicalUpdateFunction(this);
  immunity_clearance_update_function_ = new ImmunityClearanceUpdateFunction(this);
  having_drug_update_function_ = new ImmunityClearanceUpdateFunction(this);
  clinical_update_function_ = new ImmunityClearanceUpdateFunction(this);

  reporters_ = std::vector<Reporter*>();

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

/**
 * Prepare the model to be run.
 */
void Model::initialize(int job_number, const std::string& std) {
  LOG(INFO) << "Model initializing...";

  // Read the configuration and check to make sure it is valid
  LOG(INFO) << fmt::format("Read input file: {}", config_filename_);
  config_->read_from_file(config_filename_);
  if (!verify_configuration()) {
    std::cerr << "ERROR! Unable to continue execution with current configuration, see error logs.\n";
    exit(EXIT_FAILURE);
  }
  
  VLOG(1) << "Initialize Random";
  random_->initialize(config_->initial_seed_number());

  // MARKER add reporter here
  VLOG(1) << "Initialing reporter(s)...";
  try {
    if (reporter_type_.empty()) {
      add_reporter(Reporter::MakeReport(Reporter::DB_REPORTER));
    } else {
      for (const auto& type : StringHelpers::split(reporter_type_, ',')) {
        if (Reporter::ReportTypeMap.find(type) != Reporter::ReportTypeMap.end()) {
          add_reporter(Reporter::MakeReport(Reporter::ReportTypeMap[type]));
        } else {
          std::cerr << "ERROR! Unknown reporter type: " << type << std::endl;
          exit(EXIT_FAILURE);
        }
      }
    }
    for (auto* reporter : reporters_) {
      reporter->initialize(job_number, std);
    }
  } catch (std::invalid_argument &ex) {
    LOG(ERROR) << "Initialing reporter generated exception: " << ex.what();
    exit(EXIT_FAILURE);
  } catch (std::runtime_error &ex) {
    LOG(ERROR) << "Runtime error encountered while initializing reporter: " << ex.what();
    exit(EXIT_FAILURE);
  }

  VLOG(1) << "Initializing scheduler...";
  LOG(INFO) << "Starting day is " << CONFIG->starting_date();
  scheduler_->initialize(CONFIG->starting_date(), config_->total_time());
  scheduler_->set_days_between_notifications(config_->days_between_notifications());

  VLOG(1) << "Initialing initial strategy";
  set_treatment_strategy(config_->initial_strategy_id());

  VLOG(1) << "Initialing initial treatment coverage model";
  build_initial_treatment_coverage();

  VLOG(1) << "Initializing data collector";
  data_collector_->initialize();

  VLOG(1) << "Initializing population";
  population_->initialize();
  LOG(INFO) << fmt::format("Location count: {0}", CONFIG->number_of_locations());
  LOG(INFO) << fmt::format("Population size: {0}", population_->size());

  VLOG(1) << "Initializing movement model";
  config_->spatial_model()->prepare();

  VLOG(1) << "Introducing initial cases";
  population_->introduce_initial_cases();

  VLOG(1) << "Schedule for environment / population events";
  for (auto* event : config_->PreconfigEvents()) {
    scheduler_->schedule_population_event(event);
  }

  if (report_movement()) {
    // Generate a movement reporter
    Reporter* reporter = Reporter::MakeReport(Reporter::ReportType::MOVEMENT_REPORTER);
    add_reporter(reporter);
    reporter->initialize(job_number, std);

    // Get the validator and prepare it for the run
    auto& validator = MovementValidation::get_instance();
    validator.set_reporter((MovementReporter*)reporter);

    // Set the flags on the validator
    if (individual_movement_) {
      LOG(INFO) << "Tracking of individual movement enabled.";
      validator.set_individual_movement(individual_movement_);
    }
    if (cell_movement_) {
      LOG(INFO) << "Tracking of cell movement enabled.";
      validator.set_cell_movement(cell_movement_);
    }
    if (district_movement_) {
      LOG(INFO) << "Tracking of district movement enabled.";
      validator.set_district_movement(district_movement_);
    }
  }

  if (district_movement_) {
    if (!SpatialData::get_instance().has_raster(SpatialData::SpatialFileType::Districts)) {
      LOG(ERROR) << "Districts raster must be loaded to track district movements.";
      throw std::runtime_error("--mcd set without districts raster loaded.");
    }
  }

  if (dump_movement_) {
      MovementValidation::write_movement_data();    
  }
}

void Model::initialize_object_pool(const int &size) {
  VLOG(1) << fmt::format("Initialize the object pool, size: {0},", size);

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

  VLOG(1) << "Release the object pool";

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

  auto start = std::chrono::system_clock::now();
  scheduler_->run();
  auto end = std::chrono::system_clock::now();
  
  after_run();
  LOG(INFO) << "Model finished!";

  // Note the final population of the model
  LOG(INFO) << fmt::format("Final population: {0}", population_->size());

  // Note the final run-time of the model
  std::chrono::duration<double> elapsed_seconds = end-start;
  LOG(INFO) << fmt::format("Elapsed time (s): {0}", elapsed_seconds.count());
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
  data_collector()->monthly_reset();

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
  // Clean up the memory used by the model
  ObjectHelpers::delete_pointer<ClinicalUpdateFunction>(progress_to_clinical_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(immunity_clearance_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(having_drug_update_function_);
  ObjectHelpers::delete_pointer<ImmunityClearanceUpdateFunction>(clinical_update_function_);
  ObjectHelpers::delete_pointer<Population>(population_);
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

bool Model::verify_configuration() {
  // Flag for error so we can check the entire file
  bool valid = true;

  // Make sure the arrays are aligned correctly
  if (config_->death_rate_by_age_class().size() != config_->number_of_age_classes()) {
    LOG(ERROR) << fmt::format("Number of death rates ({}) does not match the number of age classes ({})!", 
      config_->death_rate_by_age_class().size(), config_->number_of_age_classes());
    valid = false;      
  }

  // This should not be zero
  if (config_->artificial_rescaling_of_population_size() <= 0) {
    LOG(ERROR) << "Population scaling cannot be less than or equal to zero.";
    valid = false;
  }

  // Issue a warning if the crude birth rate is unusually high
  if (config_->birth_rate() > 0.05) {
    LOG(WARNING) << fmt::format("Unusually high birth rate of {} ({}/1000 individuals)", 
      config_->birth_rate(), config_->birth_rate() * 1000);
  }

  // Issue an error if the crude birth rate exceeds one
  if (config_->birth_rate() >= 1) {
    LOG(ERROR) << fmt::format("Birth rate should not exceed 1.0, did you mean {} ({}/1000 individuals)?",
      config_->birth_rate() / 1000, config_->birth_rate());
      valid = false;
  }

  return valid;
}
