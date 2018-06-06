//
// Created by Nguyen Tran on 1/25/2018.
//

#include "Location.h"

namespace Spatial {
    Location::Location(int id, float latitude, float longitude, int populationSize) :
            id{id}, populationSize{populationSize}, beta{0.0f}, p_treatment_less_than_5{0.0f},
            p_treatment_more_than_5{0.0f}, coordinate{std::make_unique<Coordinate>(latitude, longitude)},
            age_distribution() {

    }

    Location::~Location() {

    }

    Location::Location(const Location &org) : id{org.id}, populationSize{org.populationSize},
                                              beta{org.beta},
                                              coordinate{std::make_unique<Coordinate>(
                                                      org.coordinate->latitude,
                                                      org.coordinate->longitude)},
                                              age_distribution(org.age_distribution) {

    }

    Location &Location::operator=(const Location &other) {
        id = other.id;
        beta = other.beta;
        populationSize = other.populationSize;
        p_treatment_less_than_5 = other.p_treatment_less_than_5;
        p_treatment_more_than_5 = other.p_treatment_more_than_5;
        coordinate = std::make_unique<Coordinate>(other.coordinate->latitude, other.coordinate->longitude);
        age_distribution = other.age_distribution;
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const Location &location) {
        os << "id: " << location.id << ", populationSize: " << location.populationSize << ", beta: " << location.beta
           << ", coordinate: " << *location.coordinate << ", age_distribution: [";

        for (double i : location.age_distribution) {
            os << i << ",";
        }
        os << "]";
        os << ", p_treatment: [" << location.p_treatment_less_than_5 << "," << location.p_treatment_more_than_5 << "]"
           << std::endl;
        return os;
    }
}