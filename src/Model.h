/* 
 * File:   Model.h
 * Author: nguyentran
 *
 * Created on March 22, 2013, 2:26 PM
 */

#ifndef MODEL_H
#define	MODEL_H

#include "PropertyMacro.h"
#include "Scheduler.h"


#include "ClinicalUpdateFunction.h"
#include "ImmunityClearanceUpdateFunction.h"
#include "ObjectPool.h"

class Scheduler;
class Population;
class ExternalPopulation;
class Config;
class Random;
class ModelDataCollector;
class Reporter;
class TMEScheduler;

class Model {
    //    DISALLOW_COPY_AND_ASSIGN_(Model);

    POINTER_PROPERTY(Config, config);
    POINTER_PROPERTY(Scheduler, scheduler);
    POINTER_PROPERTY(TMEScheduler, tme_scheduler);
    POINTER_PROPERTY(Population, population);
//    POINTER_PROPERTY(ExternalPopulation, external_population);
    POINTER_PROPERTY(Random, random);
    POINTER_PROPERTY(ModelDataCollector, data_collector);

    POINTER_PROPERTY(ClinicalUpdateFunction, progress_to_clinical_update_function);
    POINTER_PROPERTY(ImmunityClearanceUpdateFunction, immunity_clearance_update_function);
    POINTER_PROPERTY(ImmunityClearanceUpdateFunction, having_drug_update_function);
    POINTER_PROPERTY(ImmunityClearanceUpdateFunction, clinical_update_function);

    PROPERTY_REF(std::vector<Reporter*>, reporters);

    PROPERTY_REF(long, initial_seed_number);
    PROPERTY_REF(std::string, config_filename);
    PROPERTY_REF(std::string, tme_filename);
    PROPERTY_REF(std::string, override_parameter_filename);
    PROPERTY_REF(int, override_parameter_line_number); // base 1
    PROPERTY_REF(int, gui_type);
    PROPERTY_REF(bool, is_farm_output);


    static Model* MODEL;
    static Config* CONFIG;
    static Random* RANDOM;
    static Scheduler* SCHEDULER;
    static TMEScheduler* TME_SCHEDULER;
    static ModelDataCollector* DATA_COLLECTOR;
    static Population* POPULATION;
//    static Population* EXTERNAL_POPULATION;

public:
    Model(const int& object_pool_size = 100000);
    Model(const Model& orig);
    virtual ~Model();

    void initialize();

    static void initialize_object_pool(const int& size = 100000);
    void release_object_pool();

    void before_run();
    void run();
    void after_run();


    void release();


    void perform_infection_event();

    void report_begin_of_time_step();
    void report_end_of_time_step();
    void add_reporter(Reporter* reporter);



private:

};

#endif	/* MODEL_H */

