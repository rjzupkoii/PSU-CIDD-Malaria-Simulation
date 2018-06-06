/* 
 * File:   Model.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:26 PM
 */

#include "Model.h"
#include "Population.h"
#include "HelperFunction.h"
#include "Config.h"
#include "Person.h"
#include "Random.h"
#include "MDC/ModelDataCollector.h"
#include "BirthdayEvent.h"
#include "ProgressToClinicalEvent.h"
#include "EndClinicalDueToDrugResistanceEvent.h"
#include "TestTreatmentFailureEvent.h"
#include "UpdateWhenDrugIsPresentEvent.h"
#include "EndClinicalByNoTreatmentEvent.h"
#include "EndClinicalEvent.h"
#include "Drug.h"
#include "ImmuneSystem.h"
#include "SingleHostClonalParasitePopulations.h"
#include "MatureGametocyteEvent.h"
#include "MoveParasiteToBloodEvent.h"
#include "UpdateEveryKDaysEvent.h"
#include "Reporters/Reporter.h"
#include "BittingLevelGenerator.h"
#include "CirculateToTargetLocationNextDayEvent.h"
#include "ReturnToResidenceEvent.h"
#include "ClonalParasitePopulation.h"
#include "NonInfantImmuneComponent.h"
#include "InfantImmuneComponent.h"
#include "ExternalPopulation.h"
#include "SwitchImmuneComponentEvent.h"
#include "MoveToExternalPopulationEvent.h"
#include "ReturnToNormalPopulationEvent.h"
#include "TMEScheduler.h"
#include "ImportationPeriodicallyEvent.h"
#include "ImportationEvent.h"

Model *Model::MODEL = nullptr;
Config *Model::CONFIG = nullptr;
Random *Model::RANDOM = nullptr;
Scheduler *Model::SCHEDULER = nullptr;
TMEScheduler *Model::TME_SCHEDULER = nullptr;
ModelDataCollector *Model::DATA_COLLECTOR = nullptr;
Population *Model::POPULATION = nullptr;
//Population* Model::EXTERNAL_POPULATION = nullptr;

Model::Model(const int &object_pool_size) {
    initialize_object_pool(object_pool_size);
    random_ = new Random(this);
    config_ = new Config(this);
    scheduler_ = new Scheduler(this);
    tme_scheduler_ = new TMEScheduler(this);
    population_ = new Population(this);
//    external_population_ = new ExternalPopulation(this);
    data_collector_ = new ModelDataCollector(this);

    progress_to_clinical_update_function_ = new ClinicalUpdateFunction(this);
    immunity_clearance_update_function_ = new ImmunityClearanceUpdateFunction(this);
    having_drug_update_function_ = new ImmunityClearanceUpdateFunction(this);
    clinical_update_function_ = new ImmunityClearanceUpdateFunction(this);

    MODEL = this;
    CONFIG = config_;
    SCHEDULER = scheduler_;
    TME_SCHEDULER = tme_scheduler_;
    RANDOM = random_;
    DATA_COLLECTOR = data_collector_;
    POPULATION = population_;
//    EXTERNAL_POPULATION = external_population_;

    reporters_ = std::vector<Reporter *>();

    initial_seed_number_ = -1;
    config_filename_ = "config.yml";
    tme_filename_ = "tme.txt";
    override_parameter_filename_ = "";
    override_parameter_line_number_ = -1;
    gui_type_ = -1;
    is_farm_output_ = false;
}

Model::~Model() {
    //    std::cout << "Model Release" << std::endl;
    DeletePointer<ClinicalUpdateFunction>(progress_to_clinical_update_function_);
    DeletePointer<ImmunityClearanceUpdateFunction>(immunity_clearance_update_function_);
    DeletePointer<ImmunityClearanceUpdateFunction>(having_drug_update_function_);
    DeletePointer<ImmunityClearanceUpdateFunction>(clinical_update_function_);

    DeletePointer<Population>(population_);
//    DeletePointer<ExternalPopulation>(external_population_);
    DeletePointer<Scheduler>(scheduler_);
    DeletePointer<TMEScheduler>(tme_scheduler_);
    DeletePointer<ModelDataCollector>(data_collector_);

    DeletePointer<Config>(config_);
    DeletePointer<Random>(random_);

    for (Reporter *reporter: reporters_) {
        DeletePointer<Reporter>(reporter);
    }
    reporters_.clear();

    MODEL = nullptr;
    CONFIG = nullptr;
    SCHEDULER = nullptr;
    TME_SCHEDULER = nullptr;
    RANDOM = nullptr;
    DATA_COLLECTOR = nullptr;
    POPULATION = nullptr;
//    EXTERNAL_POPULATION = nullptr;

    release_object_pool();
}

void Model::initialize() {
    //Initialize Random Seed
    random_->initialize(initial_seed_number_);

    //Read input file
    config_->read_from_file(config_filename_);

    // modify parameters    
    //modify parameters && update config
    config_->override_parameters(override_parameter_filename_, override_parameter_line_number_);

    //add reporter here    
    if (gui_type_ == -1) {
        if (is_farm_output_) {
            add_reporter(Reporter::MakeReport(Reporter::FARM));
        } else {
            add_reporter(Reporter::MakeReport(Reporter::BFFARM_REPORTER));
            add_reporter(Reporter::MakeReport(Reporter::BFREPORTER));
        }
    } else {
        add_reporter(Reporter::MakeReport(Reporter::GUI));
    }

    if (override_parameter_line_number_ != -1) {
        //        add_reporter(Reporter::MakeReport(Reporter::YEARLY_REPORTER_V1));
    }


    //initialize reporters

    for (Reporter *reporter : reporters_) {
        reporter->initialize();
    }



    //initialize scheduler
    scheduler_->initialize(config_->total_time() + 2000);

    tme_scheduler_->initialize();
    if (CONFIG->tme_info().tme_starting_day != 0) {
        tme_scheduler_->read_csv(tme_filename_);
    }

    //initialize data_collector
    data_collector_->initialize();

    //initialize Population
    population_->initialize();

    //initialize infected_cases
    population_->introduce_initial_cases();

    //initialize external population
//    external_population_->initialize();


    //schedule for some special or periodic events

    for (int i = 0; i < CONFIG->importation_parasite_periodically_info().size(); i++) {
        ImportationPeriodicallyEvent::schedule_event(SCHEDULER,
                                                     CONFIG->importation_parasite_periodically_info()[i].location,
                                                     CONFIG->importation_parasite_periodically_info()[i].duration,
                                                     CONFIG->importation_parasite_periodically_info()[i].parasite_type_id,
                                                     CONFIG->importation_parasite_periodically_info()[i].number,
                                                     CONFIG->importation_parasite_periodically_info()[i].start_day);
    }

    for (int i = 0; i < CONFIG->importation_parasite_info().size(); i++) {
        ImportationEvent::schedule_event(SCHEDULER, CONFIG->importation_parasite_info()[i].location,
                                         CONFIG->importation_parasite_info()[i].time,
                                         CONFIG->importation_parasite_info()[i].parasite_type_id,
                                         CONFIG->importation_parasite_info()[i].number);
    }
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
#ifdef UNITTEST

#else
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
#endif
}

void Model::run() {
    before_run();
    scheduler_->run();
    after_run();
}

void Model::before_run() {
    //    std::cout << "Seed:" << RANDOM->seed() << std::endl;

    for (Reporter *reporter: reporters_) {
        reporter->before_run();
    }
}

void Model::after_run() {
    Model::DATA_COLLECTOR->update_after_run();

    for (Reporter *reporter : reporters_) {
        reporter->after_run();
    }
}

void Model::perform_infection_event() {
    population_->perform_infection_event();
//    external_population_->perform_infection_event();
}

void Model::report_end_of_time_step() {
    if (Model::SCHEDULER->current_time() % Model::CONFIG->report_frequency() == 0) {
        Model::DATA_COLLECTOR->perform_population_statistic();

        for (Reporter *reporter: reporters_) {
            reporter->after_time_step();
        }
    }
    //     if (Model::SCHEDULER->current_time() >= 4000) {
    //        std::cout << "end" << std::endl;
    //    }
}

void Model::report_begin_of_time_step() {

    for(Reporter *reporter :  reporters_) {
                    reporter->begin_time_step();
                }
    //    if (Model::SCHEDULER->current_time() >= 4000) {
    //        std::cout << "begin" << std::endl;
    //    }
}

void Model::add_reporter(Reporter *reporter) {
    reporters_.push_back(reporter);
    reporter->set_model(this);
}