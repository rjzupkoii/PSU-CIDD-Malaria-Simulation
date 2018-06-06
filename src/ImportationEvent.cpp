/* 
 * File:   ImportationEvent.cpp
 * Author: Merlin
 * 
 * Created on March 12, 2015, 12:23 PM
 */

#include "ImportationEvent.h"
#include "Model.h"
#include "Population.h"
#include "ImmuneSystem.h"
#include "ClonalParasitePopulation.h"
#include "Config.h"
#include "PersonIndexAll.h"
#include "Random.h"
#include "MDC/ModelDataCollector.h"
#include "PersonIndexByLocationStateAgeClass.h"

OBJECTPOOL_IMPL(ImportationEvent)

ImportationEvent::ImportationEvent(const int& location, const int& execute_at, const int& genotype_id, const int& number_of_cases) : location_(location), execute_at_(execute_at), genotype_id_(genotype_id), number_of_cases_(number_of_cases) {
}

ImportationEvent::~ImportationEvent() {
}

void ImportationEvent::schedule_event(Scheduler* scheduler, const int& location, const int& execute_at, const int& genotype_id, const int& number_of_cases) {
    if (scheduler != nullptr) {
        ImportationEvent* e = new ImportationEvent(location, execute_at, genotype_id, number_of_cases);
        //        e->set_dispatcher(p);
        e->set_dispatcher(nullptr);
        e->set_executable(true);
        e->set_time(execute_at);

        //        p->add(e);
        scheduler->schedule(e);

    }
}

void ImportationEvent::execute() {
    int number_of_importation_cases = Model::RANDOM->random_poisson(number_of_cases_);
//    std::cout << number_of_cases_ << std::endl;
    PersonIndexByLocationStateAgeClass* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

    for (int i = 0; i < number_of_importation_cases; i++) {

        int ind_ac = Model::RANDOM->random_uniform(pi->vPerson()[0][0].size());
        if (pi->vPerson()[0][0][ind_ac].size() == 0) {
            continue;
        }
        int index = Model::RANDOM->random_uniform(pi->vPerson()[0][0][ind_ac].size());
        Person* p = pi->vPerson()[0][0][ind_ac][index];

        p->immune_system()->set_increase(true);
        p->set_host_state(Person::ASYMPTOMATIC);

        ClonalParasitePopulation* blood_parasite = p->add_new_parasite_to_blood(Model::CONFIG->genotype_db()->get(genotype_id_));

        double size = Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic;

        blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
        blood_parasite->set_last_update_log10_parasite_density(size);
        blood_parasite->set_update_function(Model::MODEL->immunity_clearance_update_function());

        //        Model::POPULATION->initial_infection(pi->vPerson()[0][0][ind_ac][index], Model::CONFIG->parasite_db()->get(0));
    }

}
