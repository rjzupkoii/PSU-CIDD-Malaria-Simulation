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
#include <cassert>

// TODO: writing test 

BittingLevelGenerator::BittingLevelGenerator():
  level_density_(nullptr) {}

BittingLevelGenerator::~BittingLevelGenerator() = default;

int BittingLevelGenerator::draw_random_level(Random* random) {
  if (data_.empty()) {
    allocate(random);
  }

  const int temp = data_.back();
  data_.pop_back();
  return temp;
}

void BittingLevelGenerator::allocate(Random* random) {
  const auto chunk_size = 100000;

  const int size = level_density_->size();
  UIntVector n(size);
  random->random_multinomial(size, chunk_size, &level_density_->at(0), &n[0]);
  //
  //    for(int i :  n){
  //        std::cout << i << "\t";
  //    }
  //    std::cout<< std::endl;
  data_.clear();
  for (auto i = 0; i < size; i++) {
    for (auto j = 0; j < n[i]; j++) {
      data_.push_back(i);
    }
  }
  assert(data_.size() == chunkSize);
  random->random_shuffle(&data_[0], data_.size(), sizeof(int));
}
