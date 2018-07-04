/* 
 * File:   MultinomialDistributionGenerator.cpp
 * Author: Merlin
 * 
 * Created on July 4, 2013, 5:21 PM
 */

#include <cassert>
#include "Random.h"
#include "MultinomialDistributionGenerator.h"

// TODO: writing test

MultinomialDistributionGenerator::MultinomialDistributionGenerator():
  level_density_(nullptr) {}

MultinomialDistributionGenerator::~MultinomialDistributionGenerator() = default;

int MultinomialDistributionGenerator::draw_random_level(Random* random) {
  if (data_.empty()) {
    allocate(random);
  }

  const int temp = data_.back();
  data_.pop_back();
  return temp;
}

void MultinomialDistributionGenerator::allocate(Random* random) {
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
