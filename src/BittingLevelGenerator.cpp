/* 
 * File:   BittingLevelGenerator.cpp
 * Author: Merlin
 * 
 * Created on July 4, 2013, 5:21 PM
 */

#include "BittingLevelGenerator.h"
#include "Model.h"
#include "Config.h"
#include "Random.h"
#include <assert.h>
#include <boost/foreach.hpp>

BittingLevelGenerator::BittingLevelGenerator():data_() {
}

BittingLevelGenerator::~BittingLevelGenerator() {
}

int BittingLevelGenerator::draw_random_level(Model* model) {
    if (data_.size() == 0) {
        allocate(model);
    }

    int temp = data_.back();
    data_.pop_back();
    return temp;
}

void BittingLevelGenerator::allocate(Model* model) {
    const int chunkSize = 100000;

    int size = level_density_->size();
    UIntVector n(size);
    model->random()->random_multinomial(size, chunkSize, &level_density_->at(0), &n[0]);
//
//    for(int i :  n){
//        std::cout << i << "\t";
//    }
//    std::cout<< std::endl;
    data_.clear();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < n[i]; j++) {
            data_.push_back(i);
        }
    }
    assert(data_.size() == chunkSize);
    model->random()->random_shuffle(&data_[0], data_.size(), sizeof(int));
}