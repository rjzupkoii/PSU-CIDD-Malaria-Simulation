//
// Created by merlinvn@gmail.com on 05/01/17.
//

#ifndef MODERNCPP_IINDEXABLE_H
#define MODERNCPP_IINDEXABLE_H

#include <cstddef>
#include <array>

#define MAXIMUM_ALLOWED_NUMBER_OF_INDEXERS 10

class Indexable {
 protected:
  std::array<size_t, MAXIMUM_ALLOWED_NUMBER_OF_INDEXERS> position_in_deepest_nested_vector_;
 public:
  Indexable() : position_in_deepest_nested_vector_{} {}

  void set_position_in_deepest_nested_vector(const size_t &pos, const size_t &id) {
    position_in_deepest_nested_vector_[id] = pos;
  };

  size_t get_position_in_deepest_nested_vector(const size_t &id) const {
    return position_in_deepest_nested_vector_[id];
  };
};

#endif //MODERNCPP_IINDEXABLE_H
