/* 
 * File:   main.cpp
 * Author: Merlin
 *
 * Created on October 29, 2014, 9:51 AM
 */

#include <cstdlib>
#include "Model.h"
#include "Core/Random.h"
#include "ezOptionParser.hpp"
#include <boost/foreach.hpp>
#include "Reporter.h"
#include "HelperFunction.h"
#include "PkPdReporter.h"
#include "Genotype.h"
#include "GenotypeDatabase.h"
#include "Core/Config/Config.h"
#include "Population.h"
#include "PersonIndexAll.h"
#include "ClonalParasitePopulation.h"
#include "Events/ProgressToClinicalEvent.h"
#include "ImmuneSystem.h"
#include "IndividualsFileReporter.h"
#include <string>
#include "SCTherapy.h"

using namespace std;
void create_option_parser(ez::ezOptionParser& opt);
void handle_cli(Model* m, int argc, const char * argv[]);

void usage(ez::ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

std::string individuals_output_file = "individuals.txt";
int genotype_id = 0;

/*
 * 
 */
int main(int argc, const char * argv[]) {
    Model* m = new Model();
    handle_cli(m, argc, argv);

    m->initialize();

    BOOST_FOREACH(Reporter* reporter, m->reporters()) {
       ObjectHelpers::DeletePointer<Reporter>(reporter);
    }
    m->reporters().clear();
    m->add_reporter(new PkPdReporter());
    m->add_reporter(new IndividualsFileReporter(individuals_output_file));

    //infect all individual and schedule progress to clinical at day 0
    //    Genotype* genotype = Model::CONFIG->genotype_db()->get(12);
    Genotype* genotype = Model::CONFIG->genotype_db()->get(genotype_id);
  

    BOOST_FOREACH(Person* person, Model::POPULATION->all_persons()->vPerson()) {
        double density = Model::RANDOM->random_uniform_double(Model::CONFIG->parasite_density_level().log_parasite_density_clinical_from, Model::CONFIG->parasite_density_level().log_parasite_density_clinical_to);
        ClonalParasitePopulation* blood_parasite = person->add_new_parasite_to_blood(genotype);
        person->immune_system()->set_increase(true);
        person->set_host_state(Person::EXPOSED);
//            std::cout << "hello"<< std::endl;
        blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
        blood_parasite->set_last_update_log10_parasite_density(density);

        ProgressToClinicalEvent::schedule_event(Model::SCHEDULER, person, blood_parasite, 1);
    }


    m->run();
    delete m;
    m = NULL;
    return 0;
}

void create_option_parser(ez::ezOptionParser& opt) {
    opt.overview = "PK/PD simulation for malaria.";
    opt.syntax = "PkPdSimulation [OPTIONS]";
    opt.example = "PkPdSimulation -h\n\n";
    opt.footer = "PkPdSimulation v1.0.0 Copyright (C) 2014 OUCRU.\n";

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions.\nThere is a choice of three different layouts for description alignment. Your choice can be any one of the following to suit your style:\n\n0 - align (default)\n1 - interleave\n2 - stagger", // Help description.
            "-h", // Flag token. 
            "-help", // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
            );


    opt.add(
            "basic_config.yml", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "The config file (YAML format).", // Help description.
            "-c", // Flag token. 
            "-i", // stand for input
            "-cfg", // Flag token.
            "--config" // Flag token.
            );


    opt.add(
            "basic_config.yml", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "The individuals parasite density by day", // Help description.
            "-o", // Flag token. 
            "-output", // Flag token.
            "--output" // Flag token.
            );
    opt.add("0",
            0,
            1,
            0,
            "Initial Genotype_id",
            "-genotype_id",
            "--genotype_id"
            );
    opt.add("3",
            0,
            1,
            0,
            "Dosing day",
            "-dosing_day",
            "--dosing_day"
            );
    opt.add("-1",
            0,
            1,
            0,
            "Seed number for random generator",
            "-s",
            "--seed"
            );

}

void handle_cli(Model* m, int argc, const char * argv[]) {
    ez::ezOptionParser opt;
    create_option_parser(opt);
    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        std::string usage;
        int layout;
        opt.get("-h")->getInt(layout);
        switch (layout) {
            case 0:
                opt.getUsage(usage, 80, ez::ezOptionParser::ALIGN);
                break;
            case 1:
                opt.getUsage(usage, 80, ez::ezOptionParser::INTERLEAVE);
                break;
            case 2:
                opt.getUsage(usage, 80, ez::ezOptionParser::STAGGER);
                break;
        }
        std::cout << usage;
        exit(1);
    }

    std::vector<std::string> badOptions;
    int i;
    if (!opt.gotRequired(badOptions)) {
        for (i = 0; i < badOptions.size(); ++i)
            std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n\n";
        usage(opt);
        exit(1);
    }
    std::string config_filename = "basic_config.yml";
    if (opt.isSet("-c")) {

        opt.get("-c")->getString(config_filename);
        //        std::cout << config_filename << std::endl;
    }
    m->set_config_filename(config_filename);


    long seed = -1;
    if (opt.isSet("-s")) {
        opt.get("-s")->getLong(seed);
        //        std::cout << seed << std::endl;
    }
    m->set_initial_seed_number(seed);

    if (opt.isSet("-o")) {
        opt.get("-o")->getString(individuals_output_file);
    }
    if (opt.isSet("-genotype_id")) {
        opt.get("-genotype_id")->getInt(genotype_id);
    }

}
