/* 
 * File:   PersonIndexByLocationExternalPopulationMovingLevel.cpp
 * Author: Merlin
 * 
 * Created on August 22, 2013, 12:31 PM
 */

#include "PersonIndexByLocationExternalPopulationMovingLevel.h"
#include "Core/Config/Config.h"
#include "Model.h"

PersonIndexByLocationExternalPopulationMovingLevel::PersonIndexByLocationExternalPopulationMovingLevel(const int& no_location, const int& no_level) {
    Initialize(no_location, no_level);
}

PersonIndexByLocationExternalPopulationMovingLevel::~PersonIndexByLocationExternalPopulationMovingLevel() {
    vPerson_.clear();
}

void PersonIndexByLocationExternalPopulationMovingLevel::Initialize(const int& no_location, const int& no_level) {
    vPerson_.clear();

    PersonPtrVector ppv;
    PersonPtrVector2 ppv2;
    ppv2.assign(no_level, ppv);

    vPerson_.assign(no_location, ppv2);
}

void PersonIndexByLocationExternalPopulationMovingLevel::add(Person* p) {
    assert(vPerson_.size() > p->location() && p->location() >= 0);
    assert(vPerson_[p->location()].size() > p->external_population_moving_level());
    add(p, p->location(), p->external_population_moving_level());
}

void PersonIndexByLocationExternalPopulationMovingLevel::remove(Person* p) {
    remove_without_set_index(p);
    p->PersonIndexByLocationExternalPopulationMovingLevelHandler::set_index(-1);
}

void PersonIndexByLocationExternalPopulationMovingLevel::notify_change(Person* p, const Person::Property& property, const void* oldValue, const void* newValue) {
    switch (property) {
        case Person::LOCATION:
            change_property(p, *(int*) newValue, p->external_population_moving_level());
            break;
        case Person::EXTERNAL_POPULATION_MOVING_LEVEL:
            change_property(p, p->location(), *(int*) newValue);
            break;
        default:

            break;
    }
}

int PersonIndexByLocationExternalPopulationMovingLevel::size() const {
    return 0;
}

void PersonIndexByLocationExternalPopulationMovingLevel::add(Person* p, const int& location, const int& external_population_moving_level) {
    vPerson_[location][external_population_moving_level].push_back(p);
    p->PersonIndexByLocationExternalPopulationMovingLevelHandler::set_index(vPerson_[location][external_population_moving_level].size() - 1);
}

void PersonIndexByLocationExternalPopulationMovingLevel::remove_without_set_index(Person* p) {
    vPerson_[p->location()][p->external_population_moving_level()].back()->PersonIndexByLocationExternalPopulationMovingLevelHandler::set_index(p->PersonIndexByLocationExternalPopulationMovingLevelHandler::index());
    vPerson_[p->location()][p->external_population_moving_level()][p->PersonIndexByLocationExternalPopulationMovingLevelHandler::index()] = vPerson_[p->location()][p->external_population_moving_level()].back();
    vPerson_[p->location()][p->external_population_moving_level()].pop_back();
}

void PersonIndexByLocationExternalPopulationMovingLevel::change_property(Person* p, const int& location, const int& moving_level) {
    //remove from old position
    remove_without_set_index(p); //to save 1 set and improve performance since the index of p will changed when add

    //add to new position   
    add(p, location, moving_level);
}

void PersonIndexByLocationExternalPopulationMovingLevel::update() {
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        for (int ml = 0; ml < Model::CONFIG->external_population_circulation_information().number_of_moving_levels; ml++) {
            std::vector<Person*>(vPerson_[location][ml]).swap(vPerson_[location][ml]);
        }
    }
}