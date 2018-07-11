/* 
 * File:   ExternalPopulation.cpp
 * Author: Merlin
 * 
 * Created on August 21, 2013, 10:05 PM
 */

#include "ExternalPopulation.h"
#include "PersonIndexAll.h"
#include "PersonIndexByLocationStateAgeClass.h"
#include "HelperFunction.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Person.h"
#include "Core/Random.h"

ExternalPopulation::ExternalPopulation(Model* model) : Population(model) {


}

ExternalPopulation::~ExternalPopulation() {
//    //release memory for all persons
//    if (all_persons() != nullptr) {
//        for (int i = 0; i < all_persons()->vPerson().size(); i++) {
//           ObjectHelpers::DeletePointer<Person>(all_persons()->vPerson()[i]);
//        }
//        all_persons()->vPerson().clear();
//        set_all_persons(nullptr);
//    }
//    //release person_indexes
//
//    if (person_index_list() != nullptr) {
//
//        for( PersonIndex* person_index :  *person_index_list()) {
//           ObjectHelpers::DeletePointer<PersonIndex>(person_index);
//        }
//
//        person_index_list()->clear();
//
//        delete person_index_list();
//        set_person_index_list(nullptr);
//    }
}

void ExternalPopulation::initialize() {
        if (model() != nullptr) {
        int number_of_location = Model::CONFIG->number_of_locations();
        int number_of_hoststate = Person::NUMBER_OF_STATE;
        int number_of_ageclasses = Model::CONFIG->number_of_age_classes();

        PersonIndexByLocationStateAgeClass* p_index_by_l_s_a = new PersonIndexByLocationStateAgeClass(number_of_location, number_of_hoststate, number_of_ageclasses);
        person_index_list()->push_back(p_index_by_l_s_a);

    }
}

void ExternalPopulation::perform_infection_event() {

    std::vector<Person*> today_infections;

    for (int parasite_type_id = 0; parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {

        for (int i = 0; i < all_persons()->vPerson().size(); i++) {
            Person* p = all_persons()->vPerson()[i];
            if (p->host_state() != Person::DEAD) {
                int number_of_true_bites = Model::RANDOM->random_poisson(get_current_daily_EIR_by_location(Model::SCHEDULER->current_time(), p->location(), parasite_type_id));

                int number_of_infectious_bites = Model::RANDOM->random_binomial(Model::CONFIG->p_infection_from_an_infectious_bite(), number_of_true_bites);
                if (number_of_infectious_bites > 0) {
                    //infected one
                    if (p->host_state() != Person::EXPOSED && p->liver_parasite_type() == nullptr) {
                        p->today_infections()->push_back(parasite_type_id);
                        today_infections.push_back(p);
                    }
                }

            }
        }
    }
    //solve Multiple infections
    if (today_infections.size() == 0) return;

    for(Person* p :  today_infections) {
        p->randomly_choose_parasite();
    }

    today_infections.clear();
}

void ExternalPopulation::notify_change_in_force_of_infection(const int& location, const int& parasite_type_id, const double& relative_force_of_infection) {
    //DO NOTHING HERE
    //    std::cout << "Should be wrong"<< std::endl;

}

double ExternalPopulation::get_current_daily_EIR_by_location(const int& current_time, const int& location, const int& parasite_type_id) {
    if (parasite_type_id != 0) {
        return 0;
    }
//    double seasonality = Model::CONFIG->seasonal_factor_for_beta(current_time,
//                                                                 Model::CONFIG->external_population_circulation_information().seasonal_EIR.a[location],
//                                                                 Model::CONFIG->external_population_circulation_information().seasonal_EIR.phi[location]);

    double seasonality = 1;
    return Model::CONFIG->external_population_circulation_information().daily_EIR[location] * seasonality;
}