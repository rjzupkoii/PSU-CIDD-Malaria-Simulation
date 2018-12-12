/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Merlin
 *
 * Created on January 12, 2017, 4:31 PM
 */

#include <cstdlib>
#include <iostream>
#include "Model.h"
#include "Core/Config/Config.h"
#include "Person.h"
#include "Population.h"
#include "Core/Random.h"
#include "PkPdReporter.h"
#include "IndividualsFileReporter.h"
#include "Therapies//SCTherapy.h"
#include "PersonIndexAll.h"
#include "Strategies/IStrategy.h"
#include "Strategies/SFTStrategy.h"
#include "ImmuneSystem.h"
#include "Events/ProgressToClinicalEvent.h"
#include "MDC/ModelDataCollector.h"
#include <iomanip>
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

double getEfficacyForTherapy(Genotype* g, int therapy_id, Model* m);

typedef std::tuple<int, int, int, double, double, double> EF50Key;
typedef std::map<EF50Key, double> efficacy_map;

efficacy_map precalculate_efficacies;

/*
 * 
 */
int main(int argc, char** argv) {
    el::Configurations default_conf;
    default_conf.setToDefault();
    default_conf.setGlobally(el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureLogger("default", default_conf);
    START_EASYLOGGINGPP(argc, argv);

    auto* m = new Model();
    m->set_config_filename("input_DxG.yml");
    m->initialize();


    // initialEC50Table
    std::vector<std::vector<double>> EC50_table(Model::CONFIG->genotype_db()->size(),
                                                std::vector<double>(Model::CONFIG->drug_db()->size(), 0));
    std::cout << std::setprecision(5);

    for (const auto& genotype : *Model::CONFIG->genotype_db()) {

        std::stringstream ss;
        auto g = genotype.second;
        //    auto g = Model::CONFIG->genotype_db()->db()[0];
        ss << *(g) << "\t";
        //        auto max_therapy_id = Model::CONFIG->therapy_db().size()-1;
        auto max_therapy_id = 0;
        auto min_therapy_id = 0;
        if (argc == 3) {
            min_therapy_id = atoi(argv[1]);
            max_therapy_id = atoi(argv[2]);
        }

        for (auto therapy_id = min_therapy_id; therapy_id <= max_therapy_id; therapy_id++) {

            auto* therapy = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);
            double inferEC50[3];

            // print out efficacy
            EF50Key key;


            auto search = precalculate_efficacies.find(key);
            //
            // if (search != precalculate_efficacies.end()) {
            //   //                std::cout << search->second << std::endl;
            //   std::cout << search->second << "\t";
            // }
            // else {
            //not exist then calculate
            double efficacy = getEfficacyForTherapy(g, therapy_id, m);
            ss << efficacy << "\t";

            precalculate_efficacies.insert(std::make_pair(key, efficacy));
            // }
        }
        std::cout << ss.str() << std::endl;
    }
    delete m;
    return 0;
}

double getEfficacyForTherapy(Genotype* g, int therapy_id, Model* m) {


    auto* scTherapy = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);
    dynamic_cast<SFTStrategy*>(Model::TREATMENT_STRATEGY)->get_therapy_list().clear();
    dynamic_cast<SFTStrategy*>(Model::TREATMENT_STRATEGY)->add_therapy(scTherapy);

    //reset reporter
    for (auto reporter : m->reporters()) {
        delete reporter;
    }

    m->reporters().clear();

    m->add_reporter(new PkPdReporter());
    // m->add_reporter(new IndividualsFileReporter("out.txt"));

    auto* genotype = Model::CONFIG->genotype_db()->at(g->genotype_id());

    for (auto person : Model::POPULATION->all_persons()->vPerson()) {

        auto density = Model::CONFIG->parasite_density_level().log_parasite_density_from_liver;
        auto* blood_parasite = person->add_new_parasite_to_blood(genotype);

        person->immune_system()->set_increase(true);
        person->set_host_state(Person::EXPOSED);

        blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
        blood_parasite->set_last_update_log10_parasite_density(density);


        ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, person, blood_parasite, 1);
    }

    m->run();
    const auto result = 1 - Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[0];


    //reset

    delete Model::POPULATION;
    delete Model::SCHEDULER;
    m->set_population(new Population(m));
    Model::POPULATION = m->population();
    m->set_scheduler(new Scheduler(m));
    Model::SCHEDULER = m->scheduler();

    m->scheduler()->initialize(Model::CONFIG->starting_date(), Model::CONFIG->total_time());
    m->population()->initialize();

    //    cout << "hello" << endl;

    return result;
}
