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
#include <math.h>  // log10

#include <cstdlib>
#include <iostream>
#include "Model.h"
#include "Core/Config/Config.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Core/Random.h"
#include "PkPdReporter.h"
#include "IndividualsFileReporter.h"
#include "Therapies//SCTherapy.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Strategies/IStrategy.h"
#include "Strategies/SFTStrategy.h"
#include "Population/ImmuneSystem.h"
#include "Events/ProgressToClinicalEvent.h"
#include "MDC/ModelDataCollector.h"
#include "Therapies/Therapy.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

typedef std::vector<double> EF50Key;

typedef std::map<EF50Key, double> efficacy_map;

double getEfficacyForTherapy(Genotype* g, int therapy_id, Model* p_model);

void create_cli_option(CLI::App &app);

EF50Key get_EC50_key(SCTherapy* p_therapy, Genotype* p_genotype);


efficacy_map efficacies;

std::vector<int> therapies;
std::vector<int> genotypes;

double as_iov = -1.0;
double as_iiv = -1.0;
double as_ec50 = -1.0;

std::string input_file = "input_DxG.yml";


inline double round(double val) {
  if (val < 0) return ceil(val - 0.5);
  return floor(val + 0.5);
}

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

  auto* p_model = new Model();
  p_model->set_config_filename(input_file);
  p_model->initialize();

  if(as_iov != -1) {
      p_model->CONFIG->as_iov() = as_iov;
  } 

  if(as_iiv != -1) {
      for (auto &sd :p_model->CONFIG->drug_db()->at(0)->age_group_specific_drug_concentration_sd()) {
          sd = as_iiv;
      }
  } 

  if(as_ec50 != -1) {
      p_model->CONFIG->EC50_power_n_table()[0][0] = pow(as_ec50, p_model->CONFIG->drug_db()->at(0)->n());
  } 

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


  for (auto genotype_id = min_genotype_id; genotype_id < max_genotype_id; genotype_id++) {

    std::stringstream ss;
    auto p_genotype = (*Model::CONFIG->genotype_db())[genotype_id];
   ss << *p_genotype << "\t";

    for (auto therapy_id = min_therapy_id; therapy_id <= max_therapy_id; therapy_id++) {

      auto* therapy = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);


      EF50Key key = get_EC50_key(therapy, p_genotype);
      auto search = efficacies.find(key);
      if (search == efficacies.end()) {
        double efficacy = getEfficacyForTherapy(p_genotype, therapy_id, p_model);
        ss << efficacy << "\t";
        efficacies.insert(std::make_pair(key, efficacy));
      } else {
        ss << search->second << "\t";
      }

//      double efficacy = getEfficacyForTherapy(p_genotype, therapy_id, p_model);
//      ss << efficacy << "\t";

    }
    std::cout << ss.str() << std::endl;
  }


//    std::cout << p_model->CONFIG->drug_db()->at(0)->age_group_specific_drug_concentration_sd()[0] << std::endl;
  delete p_model;


  return 0;
}

EF50Key get_EC50_key(SCTherapy* p_therapy, Genotype* p_genotype) {
  EF50Key result;

  for (int j = 0; j < p_therapy->drug_ids.size(); ++j) {
//    drug_type->ec50_map()
    auto ec50 = Model::CONFIG->EC50_power_n_table()[p_genotype->genotype_id()][p_therapy->drug_ids[j]];
//    std::cout << p_therapy->drug_ids[j] << "-" << ec50 << std::endl;
    result.push_back(p_therapy->drug_ids[j]);
    result.push_back(ec50);
  }


  return result;
}

void create_cli_option(CLI::App &app) {
  app.add_option("-t",
                 therapies,
                 "Get efficacies for range therapies [from to]");

  app.add_option("-g",
                 genotypes,
                 "Get efficacies for range genotypes [from to]");

  app.add_option("--iov",
                 as_iov,
                   "AS inter-occasion-variability");
  app.add_option("--iiv",
                   as_iiv,
                   "AS inter-individual-variability");
  app.add_option("--ec50",
                  as_ec50,
                  "EC50 for AS on C580 only");

  app.add_option("-i,--input", input_file, "Input filename for DxG");
}

double getEfficacyForTherapy(Genotype* g, int therapy_id, Model* p_model) {

  auto* scTherapy = dynamic_cast<SCTherapy*>(Model::CONFIG->therapy_db()[therapy_id]);
  dynamic_cast<SFTStrategy*>(Model::TREATMENT_STRATEGY)->get_therapy_list().clear();
  dynamic_cast<SFTStrategy*>(Model::TREATMENT_STRATEGY)->add_therapy(scTherapy);

  //reset reporter
  for (auto reporter : p_model->reporters()) {
    delete reporter;
  }

  p_model->reporters().clear();

  p_model->add_reporter(new PkPdReporter());
  // p_model->add_reporter(new IndividualsFileReporter("out.txt"));

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

  p_model->run();
  const auto result = 1 - Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[0];


  delete Model::POPULATION;
  delete Model::SCHEDULER;
  p_model->set_population(new Population(p_model));
  Model::POPULATION = p_model->population();
  p_model->set_scheduler(new Scheduler(p_model));
  Model::SCHEDULER = p_model->scheduler();

  p_model->scheduler()->initialize(Model::CONFIG->starting_date(), Model::CONFIG->total_time());
  p_model->population()->initialize();


  return result;
}
