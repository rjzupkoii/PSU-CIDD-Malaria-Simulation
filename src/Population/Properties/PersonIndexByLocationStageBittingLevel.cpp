//
// Created by ktt5121 on 9/8/2021.
//

#include "PersonIndexByLocationStageBittingLevel.h"
#include "Core/Config/Config.h"
#include "Model.h"
#include <cassert>
#include <cmath>

PersonIndexByLocationStageBittingLevel::PersonIndexByLocationStageBittingLevel(const int &no_location, const int &no_host_state,
                                                                       const int &bitting_level) {
    Initialize(no_location, no_host_state, bitting_level);
}

PersonIndexByLocationStageBittingLevel::~PersonIndexByLocationStageBittingLevel() {

}

void PersonIndexByLocationStageBittingLevel::Initialize(const int &no_location, const int &no_host_state,
                                                    const int &bitting_level) {
    vPerson_.clear();

    PersonPtrVector ppv;

    PersonPtrVector2 ppv2;
    ppv2.assign(bitting_level, ppv);

    PersonPtrVector3 ppv3;
    ppv3.assign(no_host_state, ppv2);

    vPerson_.assign(no_location, ppv3);
}

void PersonIndexByLocationStageBittingLevel::add(Person *p) {
    assert(vPerson_.size() > static_cast<std::size_t>(p->location()) && p->location() >= 0);
    assert(vPerson_[p->location()].size() > p->host_state());
    assert(vPerson_[p->location()][p->host_state()].size() > static_cast<std::size_t>(p->bitting_level()));
    assert(p->bitting_level() >= 0);
    add(p, p->location(), p->host_state(), p->bitting_level());
}

void PersonIndexByLocationStageBittingLevel::add(Person *p, const int &location, const Person::HostStates &host_state, const int &bitting_level) {
    auto& reference = vPerson_[location][host_state][bitting_level];
    reference.push_back(p);
    p->PersonIndexByLocationStageBittingLevelHandler::set_index(reference.size() - 1);
}

void PersonIndexByLocationStageBittingLevel::remove(Person *p) {
    remove_without_set_index(p);
    p->PersonIndexByLocationStageBittingLevelHandler::set_index(-1);
}

void PersonIndexByLocationStageBittingLevel::remove_without_set_index(Person *p) {
    auto& reference = vPerson_[p->location()][p->host_state()][p->bitting_level()];
    reference.back()->PersonIndexByLocationStageBittingLevelHandler::set_index(p->PersonIndexByLocationStageBittingLevelHandler::index());
    reference[p->PersonIndexByLocationStageBittingLevelHandler::index()] = reference.back();
    reference.pop_back();
}

std::size_t PersonIndexByLocationStageBittingLevel::size() const {
    return 0;
}

void
PersonIndexByLocationStageBittingLevel::notify_change(Person *p, const Person::Property &property, const void *oldValue,
                                                  const void *newValue) {

    switch (property) {
        case Person::LOCATION:change_property(p, *(int *) newValue, p->host_state(), p->bitting_level());
            break;
        case Person::HOST_STATE:change_property(p, p->location(), *(Person::HostStates *) newValue, p->bitting_level());
            break;
        case Person::BITTING_LEVEL:change_property(p, p->location(), p->host_state(), *(int *) newValue);
            break;
        default:break;
    }

}

void PersonIndexByLocationStageBittingLevel::change_property(Person *p, const int &location,
                                                         const Person::HostStates &host_state, const int &bitting_level) {
    //remove from old position
    remove_without_set_index(p); //to save 1 set and improve performance since the index of p will changed when add

    //add to new position
    add(p, location, host_state, bitting_level);
}

// Iterate through the data structure and force std::vector to reallocate the underlying memory.
void PersonIndexByLocationStageBittingLevel::defragment() {
    for (std::size_t location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        for (int hs = 0; hs < Person::NUMBER_OF_STATE; hs++) {
            for (std::size_t bl = 0; bl < Model::CONFIG->relative_bitting_info().number_of_biting_levels; bl++) {
                std::vector<Person *>(vPerson_[location][hs][bl]).swap(vPerson_[location][hs][bl]);
            }
        }
    }
}