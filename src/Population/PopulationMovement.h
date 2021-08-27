/*
 * PopulationMovement.h
 *
 * This class encapsulates the logic necessary to circulation the individuals in the simulation when called (e.g., daily
 * movement) when the perform_circulation method is called. If the simulation and the spatial movement model support
 * parallel processing, then parallel pathways will be used when possible, otherwise the operations will be executed in
 * serial fashion.
 */
#ifndef POPULATIONMOVEMENT_H
#define POPULATIONMOVEMENT_H

#include <mutex>

#include "Population.h"
#include "Properties/PersonIndexByLocationMovingLevel.h"

class PopulationMovement {
public:
    struct Circulation {
        int _from;
        int _to;
        int _count;

        Circulation(int from, int to, int count) : _from(from), _to(to), _count(count) { }
    };

private:
  // Movements recorded during parallel execution
  mutable std::mutex circulation_mutex;
  std::vector<Circulation> movements;

  // Index of all the individuals in the simulation, by location index
  PersonIndexByLocationMovingLevel* _personIndex;

  // Add the circulation event to the movements vector
  void add_circulation(Circulation circulation);

  // Determine the probability of movement from a given location to another, based upon the spatial model defined for
  // the simulation. Calculations are done in parallel and stored in the movements vector
  static void perform_circulation_from_location(int from, int count, PopulationMovement* movement);

  // Perform the daily population movement using parallel processing.
  void perform_circulation_parallel(Population* population);

  // Perform the daily population movement using serial processing.
  void perform_circulation_serial(Population* population);

  // Perform
  void perform_circulation_for_location(int from, int target, unsigned int count, std::vector<Person*>& today_circulations);

public:
  // Perform the circulation step on the population
  void perform_circulation(Population* population);
};

#endif
