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
#include <iomanip>
#include <CLI11.hpp>

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
#include "Therapies/Therapy.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

double getEfficacyForTherapy(Genotype* g, int therapy_id, Model* m);

void create_cli_option(CLI::App &app);

typedef std::tuple<int, int, int, double, double, double> EF50Key;
typedef std::map<EF50Key, double> efficacy_map;

efficacy_map efficacies;

std::vector<int> therapies;
std::vector<int> genotypes;

std::string input_file = "input_DxG.yml";

/*
 * 
 */
int main(int argc, char** argv) {
  CLI::App app{"PKPD model"};
  create_cli_option(app);
  CLI11_PARSE(app, argc, argv);


  //Turn off logger
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.setGlobally(el::ConfigurationType::Enabled, "false");
  el::Loggers::reconfigureLogger("default", default_conf);
  START_EASYLOGGINGPP(argc, argv);

  auto* m = new Model();
  m->set_config_filename(input_file);
  m->initialize();


  // initialEC50Table
  std::vector<std::vector<double>> EC50_table(Model::CONFIG->genotype_db()->size(),
                                              std::vector<double>(Model::CONFIG->drug_db()->size(), 0));
  std::cout << std::setprecision(5);
  int max_therapy_id{0}, min_therapy_id{0};

  if (therapies.empty()) {
    min_therapy_id = 0;
    max_therapy_id = 0;
  } else if (therapies.size() == 1) {
    min_therapy_id = therapies[0];
    max_therapy_id = therapies[0];
  } else if (therapies.size() == 2) {
    min_therapy_id = therapies[0];
    max_therapy_id = therapies[1];
  }

  int max_genotype_id{0}, min_genotype_id{0};
  if (genotypes.empty()) {
    min_genotype_id = 0;
    max_genotype_id = Model::CONFIG->number_of_parasite_types();
  } else if (genotypes.size() == 1) {
    min_genotype_id = genotypes[0];
    max_genotype_id = genotypes[0];
  } else if (genotypes.size() == 2) {
    min_genotype_id = genotypes[0];
    max_genotype_id = genotypes[1];
  }


  std::cout << "ID\tGenotype";
  for (auto therapy_id = min_therapy_id; therapy_id <= max_therapy_id; therapy_id++) {
    std::cout << "\t" << *Model::CONFIG->therapy_db()[therapy_id];
  }

  std::cout << std::endl;

  for (auto genotype_id = min_genotype_id; genotype_id <= max_genotype_id; genotype_id++) {

    std::stringstream ss;
    auto g = (*Model::CONFIG->genotype_db())[genotype_id];
    ss << *g << "\t";

    for (auto therapy_id = min_therapy_id; therapy_id <= max_therapy_id; therapy_id++) {

      auto* therapy = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);

      // print out efficacy
      EF50Key key;

      auto search = efficacies.find(key);
      //
      // if (search != efficacies.end()) {
      //   //                std::cout << search->second << std::endl;
      //   std::cout << search->second << "\t";
      // }
      // else {
      //not exist then calculate
      double efficacy = getEfficacyForTherapy(g, therapy_id, m);
      ss << efficacy << "\t";

      efficacies.insert(std::make_pair(key, efficacy));
      // }
    }
    std::cout << ss.str() << std::endl;
  }
  delete m;
  return 0;
}

void create_cli_option(CLI::App &app) {
  app.add_option("-t",
                 therapies,
                 "Get efficacies for range therapies [from to]");

  app.add_option("-g",
                 genotypes,
                 "Get efficacies for range genotypes [from to]");
  app.add_option("-i,--input", input_file, "Output filename for PPQ");
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


  delete Model::POPULATION;
  delete Model::SCHEDULER;
  m->set_population(new Population(m));
  Model::POPULATION = m->population();
  m->set_scheduler(new Scheduler(m));
  Model::SCHEDULER = m->scheduler();

  m->scheduler()->initialize(Model::CONFIG->starting_date(), Model::CONFIG->total_time());
  m->population()->initialize();


  return result;
}
