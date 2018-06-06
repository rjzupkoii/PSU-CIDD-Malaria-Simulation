/* 
 * File:   ResistanceTracker.cpp
 * Author: Merlin
 * 
 * Created on September 24, 2013, 10:00 AM
 */

#include "ResistanceTracker.h"
#include "../Model.h"
#include "../Config.h"
#include "../IntGenotype.h"
#include "../IntGenotypeDatabase.h"
#include <numeric>
#include <boost/foreach.hpp>


ResistanceTracker::ResistanceTracker() : total_{0} {
}

ResistanceTracker::~ResistanceTracker() {
}

void ResistanceTracker::make_resistance_profile(std::vector<int> &vResistanceID, const int &size) {
    vResistanceID.clear();

    for (IntGenotypePtrMap::value_type &i : Model::CONFIG->genotype_db()->db()) {
        if (i.second->number_of_resistance_position() == size) {
            vResistanceID.push_back(i.first);
        }
    }


}

void ResistanceTracker::make_arterminsinin_resistance_profile(std::vector<int> &vResistanceID) {
    vResistanceID.clear();

    for (IntGenotypePtrMap::value_type &i:  Model::CONFIG->genotype_db()->db()) {
        if (i.second->gene_expression()[0] != 0) {
            //            std::cout << i.first << std::endl;
            vResistanceID.push_back(i.first);
        }
    }
}

void ResistanceTracker::initialize() {
    parasite_population_count_.assign(Model::CONFIG->number_of_parasite_types(), 0);
    parasite_population_count_by_location_.assign(Model::CONFIG->number_of_locations(),
                                                  LongVector(Model::CONFIG->number_of_parasite_types(), 0));

    DrugTypePtrMap &drug_db = Model::CONFIG->drug_db()->drug_db();
    IntVector drugs_used;


    all_resistance_id_ = Model::CONFIG->genotype_db()->get(
            Model::CONFIG->genotype_db()->db().size() - 1)->genotype_id();
    //    
    make_resistance_profile(single_resistance_ids_, 1);
    make_resistance_profile(double_resistance_ids_, 2);
//    for (auto i : double_resistance_ids_) {
//        std::cout << i << std::endl;
//    }
    make_resistance_profile(tripple_resistance_ids_, 3);
    make_resistance_profile(quadruple_resistance_ids_, 4);
    make_resistance_profile(quintuple_resistance_ids_, 5);

    //TODO: add 6 or more all resistance pos

    make_arterminsinin_resistance_profile(artemisinin_ids_);

    total_resistance_ids_.clear();
    total_resistance_ids_.push_back(&single_resistance_ids_);
    total_resistance_ids_.push_back(&double_resistance_ids_);
    total_resistance_ids_.push_back(&tripple_resistance_ids_);
    total_resistance_ids_.push_back(&quadruple_resistance_ids_);
    total_resistance_ids_.push_back(&quintuple_resistance_ids_);

    any_single_tracking_time_.clear();
    all_single_tracking_time_.clear();
    any_double_tracking_time_.clear();
    all_double_tracking_time_.clear();
    any_triple_tracking_time_.clear();
    all_triple_tracking_time_.clear();
    any_quadruple_tracking_time_.clear();
    all_quadruple_tracking_time_.clear();
    any_quintuple_tracking_time_.clear();
    all_quintuple_tracking_time_.clear();
    total_tracking_time_.clear();
    artemisinin_tracking_time_.clear();


    tracking_values_.clear();
    tracking_values_.push_back(0.001);
    tracking_values_.push_back(0.005);
    tracking_values_.push_back(0.01);
    tracking_values_.push_back(0.05);
    tracking_values_.push_back(0.10);
    tracking_values_.push_back(0.25);

    for (int i = 0; i < tracking_values_.size(); i++) {
        any_single_tracking_time_.push_back(-1);
        all_single_tracking_time_.push_back(-1);
        any_double_tracking_time_.push_back(-1);
        all_double_tracking_time_.push_back(-1);
        any_triple_tracking_time_.push_back(-1);
        all_triple_tracking_time_.push_back(-1);
        any_quadruple_tracking_time_.push_back(-1);
        all_quadruple_tracking_time_.push_back(-1);
        any_quintuple_tracking_time_.push_back(-1);
        all_quintuple_tracking_time_.push_back(-1);
        total_tracking_time_.push_back(-1);
        artemisinin_tracking_time_.push_back(-1);
    }
}

void ResistanceTracker::increase(const int &id, const int &location) {
//    std::cout << id << std::endl;
    parasite_population_count_[id] += 1;
    parasite_population_count_by_location_[location][id] += 1;
    total_ += 1;
}

void ResistanceTracker::decrease(const int &id, const int &location) {
    parasite_population_count_[id] -= 1;
    parasite_population_count_by_location_[location][id] -= 1;
    total_ -= 1;
}

void ResistanceTracker::change(const int &from, const int &to, const int &location) {
    parasite_population_count_[from] -= 1;
    parasite_population_count_[to] += 1;

    parasite_population_count_by_location_[location][from] -= 1;
    parasite_population_count_by_location_[location][to] += 1;

}

void ResistanceTracker::change_location(const int &genotype_id, const int &from_location, const int &to_location) {
    parasite_population_count_by_location_[from_location][genotype_id] -= 1;
    parasite_population_count_by_location_[to_location][genotype_id] += 1;
}

void ResistanceTracker::update_resistance_tracker() {

    for (int i = 0; i < tracking_values_.size(); i++) {
        if (any_single_tracking_time_[i] == -1) {
            update_time_value(any_single_tracking_time_[i], max_fraction_resistance(single_resistance_ids_),
                              tracking_values_[i]);
        }
        if (all_single_tracking_time_[i] == -1) {
            update_time_value(all_single_tracking_time_[i], min_fraction_resistance(single_resistance_ids_),
                              tracking_values_[i]);
        }
        if (any_double_tracking_time_[i] == -1) {
            update_time_value(any_double_tracking_time_[i], max_fraction_resistance(double_resistance_ids_),
                              tracking_values_[i]);
        }
        if (all_double_tracking_time_[i] == -1) {
            update_time_value(all_double_tracking_time_[i], min_fraction_resistance(double_resistance_ids_),
                              tracking_values_[i]);
        }

        if (any_triple_tracking_time_[i] == -1) {
            update_time_value(any_triple_tracking_time_[i], max_fraction_resistance(tripple_resistance_ids_),
                              tracking_values_[i]);
        }
        if (all_triple_tracking_time_[i] == -1) {
            update_time_value(all_triple_tracking_time_[i], min_fraction_resistance(tripple_resistance_ids_),
                              tracking_values_[i]);
        }
        if (any_quadruple_tracking_time_[i] == -1) {
            update_time_value(any_quadruple_tracking_time_[i], max_fraction_resistance(quadruple_resistance_ids_),
                              tracking_values_[i]);
        }
        if (all_quadruple_tracking_time_[i] == -1) {
            update_time_value(all_quadruple_tracking_time_[i], min_fraction_resistance(quadruple_resistance_ids_),
                              tracking_values_[i]);
        }
        if (any_quintuple_tracking_time_[i] == -1) {
            update_time_value(any_quintuple_tracking_time_[i], max_fraction_resistance(quintuple_resistance_ids_),
                              tracking_values_[i]);
        }
        if (all_quintuple_tracking_time_[i] == -1) {
            update_time_value(all_quintuple_tracking_time_[i], min_fraction_resistance(quintuple_resistance_ids_),
                              tracking_values_[i]);
        }


        calculate_total_resistance_frequency();

        if (total_tracking_time_[i] == -1) {
            update_time_value(total_tracking_time_[i], total_resistance_frequency_, tracking_values_[i]);
        }

        if (artemisinin_tracking_time_[i] == -1) {
            update_time_value(artemisinin_tracking_time_[i], sum_fraction_resistance(artemisinin_ids_),
                              tracking_values_[i]);
        }
    }
}

double ResistanceTracker::calculate_total_resistance_frequency() {


    const int size = Model::CONFIG->genotype_db()->get(0)->gene_expression().size();

    //get weighted sum
    total_resistance_frequency_ = 0;
    for (int c = 0; c < total_resistance_ids_.size(); c++) {
        double sub_sum = 0;
        if (c == size - 1) {
            sub_sum += parasite_population_count_[all_resistance_id_];
        } else {
            for (int j = 0; j < total_resistance_ids_[c]->size(); j++) {
                sub_sum += parasite_population_count_[total_resistance_ids_[c]->at(j)];
            }
        }
        total_resistance_frequency_ += (c + 1) * sub_sum;
    }
    total_resistance_frequency_ = (total_resistance_frequency_ / total_) / size;

    return total_resistance_frequency_;
}

double ResistanceTracker::max_fraction_resistance(const IntVector &resitance_ids) {
    double max = 0;

    for (int i = 0; i < resitance_ids.size(); i++) {
        //        double fraction = (total_ == 0) ? 0 : parasite_population_count_[resitance_ids[i]] / (double) total_;
        if (parasite_population_count_[resitance_ids[i]] > max) {
            max = parasite_population_count_[resitance_ids[i]];
        }
    }
    max = (total_ == 0) ? 0 : max / (double) total_;
    return max;
}

double ResistanceTracker::min_fraction_resistance(const IntVector &resitance_ids) {
    double min = 1;

    for (int i = 0; i < resitance_ids.size(); i++) {
        if (parasite_population_count_[resitance_ids[i]] < min) {
            min = parasite_population_count_[resitance_ids[i]];
        }
    }
    min = (total_ == 0) ? 0 : min / (double) total_;
    return min;
}

double ResistanceTracker::sum_fraction_resistance(const IntVector &resitance_ids) {
    double sum = 0;

    for (int i = 0; i < resitance_ids.size(); i++) {
        sum += parasite_population_count_[resitance_ids[i]];
    }
    sum = (total_ == 0) ? 0 : sum / (double) total_;
    return sum;
}

void ResistanceTracker::update_time_value(int &tracking_time, const double &value, const double &check_value) {

    if (tracking_time == -1) {
        if (value >= check_value) {
            tracking_time = Model::SCHEDULER->current_time();
        }
    }
}
