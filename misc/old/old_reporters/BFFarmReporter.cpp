//
// Created by Nguyen Tran on 3/5/2018.
//

#include "BFFarmReporter.h"
#include "Model.h"
#include "Population.h"
#include "Core/Random.h"
#include "Core/Config/Config.h"
#include "PersonIndexAll.h"

BFFarmReporter::BFFarmReporter() {

}

BFFarmReporter::~BFFarmReporter() {

}

void BFFarmReporter::initialize() {

}

void BFFarmReporter::before_run() {
//    std::cout << Model::RANDOM->seed() << std::endl;

}

void BFFarmReporter::begin_time_step() {

}

void BFFarmReporter::monthly_report() {

}

void BFFarmReporter::after_run() {
//    for (auto p : Model::POPULATION->get_person_index<PersonIndexAll>()->vPerson()) {
//        if (p->host_state() != Person::DEAD ) {
//            std::cout << p->number_of_trips_taken() << "\t";
//        }
//    }
//    std::cout << std::endl;

  outout_parameters();

  std::cout << std::endl;

}

void BFFarmReporter::outout_parameters() {
//    std::cout << boost::format("%1%\t%2%\t")
//                 % Model::RANDOM->seed()
//                 % Model::CONFIG->number_of_locations();

}
