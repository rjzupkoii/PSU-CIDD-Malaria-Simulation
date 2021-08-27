/*
 *
 */
#ifndef POPULATIONMOVEMENT_H
#define POPULATIONMOVEMENT_H

#include <mutex>

#include "Population.h"

class PopulationMovement {
public:
    struct Circulation {
        int from;
        int to;
        int count;

        Circulation(int from, int to, int count) {
          this->from = from;
          this->to = to;
          this->count = count;
        }
    };

private:

  mutable std::mutex circulation_mutex;
  std::vector<Circulation> movements;

  static void perform_circulation_location(int from, int count, PopulationMovement* movement);

  void add_circulation(Circulation circulation);

public:

  // Perform the daily population movement using serial processing.
  void perform_circulation_serial() { }

  // Perform the daily population movement using parallel processing.
  void perform_circulation_parallel(Population* population);


};

#endif
