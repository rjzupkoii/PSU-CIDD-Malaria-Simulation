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
#include "SCTherapy.h"
#include "PersonIndexAll.h"
#include "Strategies/IStrategy.h"
#include "Strategies/SFTStrategy.h"
#include "ImmuneSystem.h"
#include "Events/ProgressToClinicalEvent.h"
#include "ModelDataCollector.h"

using namespace std;

double getEfficacyForTherapy(Genotype* g, int therapy_id, double inferEC50[], Model* m);

typedef std::tuple<int, int, int, double, double, double> EF50Key;
typedef std::map<EF50Key, double> efficacy_map;

efficacy_map precalculate_efficacies;

/*
 * 
 */
int main(int argc, char** argv) {

    Model* m = new Model();
    m->set_config_filename("input.yml");
    m->initialize();


    // initialEC50Table
    std::vector<std::vector<double> > EC50_table(Model::CONFIG->genotype_db()->db().size(), std::vector<double>(Model::CONFIG->drug_db()->drug_db().size(), 0));

    for (int g_id = 0; g_id < Model::CONFIG->genotype_db()->db().size(); g_id++) {
        for (int i = 0; i < Model::CONFIG->drug_db()->drug_db().size(); i++) {
            EC50_table[g_id][i] = Model::CONFIG->drug_db()->drug_db()[i]->inferEC50(Model::CONFIG->genotype_db()->db()[g_id]);
        }
    }

    //adjust ec50 table




    std::cout << std::setprecision(5);

    for (const auto& genotype : Model::CONFIG->genotype_db()->db()) {
        auto g = genotype.second;
        //    auto g = Model::CONFIG->genotype_db()->db()[0];
        std::cout << *(g) << "\t";
        //        auto max_therapy_id = Model::CONFIG->therapy_db().size()-1;
        auto max_therapy_id = 0;
        auto min_therapy_id = 0;
        if (argc == 3) {
            min_therapy_id = atoi(argv[1]);
            max_therapy_id = atoi(argv[2]);
        }

        for (int therapy_id = min_therapy_id; therapy_id <= max_therapy_id; therapy_id++) {

            SCTherapy* therapy = dynamic_cast<SCTherapy*> (Model::CONFIG->therapy_db()[therapy_id]);
            double inferEC50[3];

            // print out efficacy
            EF50Key key;
            if (therapy->drug_ids().size() == 3) {
                key = std::make_tuple(therapy->drug_ids()[0], therapy->drug_ids()[1], therapy->drug_ids()[1], EC50_table[g->genotype_id()][therapy->drug_ids()[0]], EC50_table[g->genotype_id()][therapy->drug_ids()[1]], EC50_table[g->genotype_id()][therapy->drug_ids()[2]]);
                inferEC50[0] = EC50_table[g->genotype_id()][therapy->drug_ids()[0]];
                inferEC50[1] = EC50_table[g->genotype_id()][therapy->drug_ids()[1]];
                inferEC50[2] = EC50_table[g->genotype_id()][therapy->drug_ids()[2]];
                //                std::cout << inferEC50[0] << "\t" << inferEC50[1] << "\t";
            } else if (therapy->drug_ids().size() == 2) {
                key = std::make_tuple(therapy->drug_ids()[0], therapy->drug_ids()[1], -1, EC50_table[g->genotype_id()][therapy->drug_ids()[0]], EC50_table[g->genotype_id()][therapy->drug_ids()[1]], -1);
                inferEC50[0] = EC50_table[g->genotype_id()][therapy->drug_ids()[0]];
                inferEC50[1] = EC50_table[g->genotype_id()][therapy->drug_ids()[1]];
                //                std::cout << inferEC50[0] << "\t" << inferEC50[1] << "\t";
            } else {
                key = std::make_tuple(therapy->drug_ids()[0], -1, -1, EC50_table[g->genotype_id()][therapy->drug_ids()[0]], -1, -1);
                inferEC50[0] = EC50_table[g->genotype_id()][therapy->drug_ids()[0]];
                //                std::cout << inferEC50[0] << "\t";
            }

            auto search = precalculate_efficacies.find(key);

            if (search != precalculate_efficacies.end()) {
                //                std::cout << search->second << std::endl;
                std::cout << search->second << "\t";
            } else {
                //not exist then calculate
                double efficacy = getEfficacyForTherapy(g, therapy_id, inferEC50, m);
                std::cout << efficacy << "\t";
                precalculate_efficacies.insert(std::make_pair(key, efficacy));
            }
        }
        std::cout << std::endl;
    }
    delete m;
    return 0;
}

double getEfficacyForTherapy(Genotype* g, int therapy_id, double inferEC50[], Model* m) {


    SCTherapy* scTherapy = dynamic_cast<SCTherapy*> (Model::CONFIG->therapy_db()[therapy_id]);
    ((SFTStrategy*) Model::TREATMENT_STRATEGY)->get_therapy_list().clear();
    ((SFTStrategy*) Model::TREATMENT_STRATEGY)->add_therapy(scTherapy);

    for (int i = 0; i < scTherapy->drug_ids().size(); i++) {
        //re-config EC50 table for genotype id and drug_id
        m->CONFIG->EC50_power_n_table()[g->genotype_id()][scTherapy->drug_ids()[i]] = pow(inferEC50[i], m->CONFIG->drug_db()->get(scTherapy->drug_ids()[i])->n());
    }

    //reset reporter
    for (auto reporter : m->reporters()) {
        delete reporter;
    }

    m->reporters().clear();

    m->add_reporter(new PkPdReporter());
    m->add_reporter(new IndividualsFileReporter("out.txt"));

    Genotype* genotype = Model::CONFIG->genotype_db()->get(g->genotype_id());

    for (auto person : Model::POPULATION->all_persons()->vPerson()) {
        double density = Model::CONFIG->parasite_density_level().log_parasite_density_from_liver;
        ClonalParasitePopulation* blood_parasite = person->add_new_parasite_to_blood(genotype);

        person->immune_system()->set_increase(true);
        person->set_host_state(Person::EXPOSED);

        blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
        blood_parasite->set_last_update_log10_parasite_density(density);


        ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, person, blood_parasite, 1);
    }

    m->run();
    double result = 1 - Model::DATA_COLLECTOR->current_TF_by_location()[0];


    //reset

    delete Model::POPULATION;
    delete Model::SCHEDULER;
    m->set_population(new Population(m));
    Model::POPULATION = m->population();
    m->set_scheduler(new Scheduler(m));
    Model::SCHEDULER = m->scheduler();

    m->scheduler()->initialize(Model::CONFIG->total_time() + 2000);
    m->population()->initialize();

    //    cout << "hello" << endl;

    return result;
}

