/*
 * PopulationMovement.cpp
 *
 * Implement the population movement class and logic.
 */
#include "PopulationMovement.h"

#include "easylogging++.h"

#include "Core/Config/Config.h"
#include "Core/ParallelJobs.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Spatial/SpatialModel.hxx"

void PopulationMovement::perform_circulation(Population* population) {
  _personIndex = population->get_person_index<PersonIndexByLocationMovingLevel>();
  if (ParallelJobs::get_instance().running() && Model::CONFIG->spatial_model()->parallel_compliant()) {
    perform_circulation_parallel(population);
  } else {
    perform_circulation_serial(population);
  }
}

void PopulationMovement::perform_circulation_parallel(Population* population) {
  for (auto from = 0; from < Model::CONFIG->number_of_locations(); from++) {
    // What percentage of the population is moving
    auto percentage = static_cast<double>(population->size(from)) * Model::CONFIG->circulation_info().circulation_percent;
    if (percentage == 0) { continue; }

    // What's the count of the population that is moving
    const auto count = Model::RANDOM->random_poisson(percentage);
    if (count == 0) { continue; }

    // Perform the remainder of the circulation for this cell in parallel
    ParallelJobs::get_instance().submit(perform_circulation_from_location, from, count, this);
  }

  // Wait for everything to catch up before continuing
  while (ParallelJobs::get_instance().work_pending());

  // Perform all the circulation operations
  PersonPtrVector circulations;
  for (auto movement : movements) {
    perform_circulation_for_location(movement._from, movement._to, movement._count, circulations);
  }
  for (auto* person : circulations) {
    person->randomly_choose_target_location();
  }

  // Clean-up after ourselves
  movements.clear();
}

void PopulationMovement::perform_circulation_from_location(int from, int count, PopulationMovement* movement) {
  // Determine the likelihood of travel from the current location
  DoubleVector outMovement(Model::CONFIG->number_of_locations(), 0);
  outMovement = Model::CONFIG->spatial_model()->get_v_relative_out_movement_to_destination(from,
    static_cast<int>(Model::CONFIG->number_of_locations()), Model::CONFIG->spatial_distance_matrix()[from],
    Model::DATA_COLLECTOR->popsize_residence_by_location());

  // Determine the number of individuals that are leaving each location
  std::vector<unsigned int> leavers(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));
  Model::RANDOM->random_multinomial(static_cast<int>(outMovement.size()), static_cast<unsigned int>(count),&outMovement[0], &leavers[0]);

  // Generate the circulation events for each location
  PersonPtrVector today_circulations;
  for (std::size_t target = 0; target < Model::CONFIG->number_of_locations(); target++) {
    if (leavers[target] == 0) { continue; }
    movement->add_circulation(Circulation(from, static_cast<int>(target), static_cast<int>(leavers[target])));
  }
}

void PopulationMovement::add_circulation(PopulationMovement::Circulation circulation) {
  std::scoped_lock lock(circulation_mutex);
  movements.emplace_back(circulation);
}

void PopulationMovement::perform_circulation_serial(Population* population) {
  PersonPtrVector today_circulations;

  // Grab a copy of residents by location
  const auto residents_by_location = Model::DATA_COLLECTOR->popsize_residence_by_location();

  //for each location
  // get number of circulations based on size * circulation_percent
  // distributes that number into others location based of other location size
  // for each number in that list select an individual, and schedule a movement event on next day
  for (auto from = 0; from < Model::CONFIG->number_of_locations(); from++) {

    // Determine how much of the population is moving
    auto percentage = static_cast<double>(population->size(from)) * Model::CONFIG->circulation_info().circulation_percent;
    if (percentage == 0) { continue; }
    const auto count = Model::RANDOM->random_poisson(percentage);
    if (count == 0) { continue; }

    // Determine the likelihood of moving to any given location
    DoubleVector outMovement(Model::CONFIG->number_of_locations(), 0);
    outMovement = Model::CONFIG->spatial_model()->get_v_relative_out_movement_to_destination(from,
      static_cast<int>(Model::CONFIG->number_of_locations()), Model::CONFIG->spatial_distance_matrix()[from],
      residents_by_location);

    // Determine who is going to be moving to the target location
    std::vector<unsigned int> leavers(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));
    Model::RANDOM->random_multinomial(static_cast<int>(outMovement.size()), static_cast<unsigned int>(count), &outMovement[0], &leavers[0]);

    // Generate the circulation events for each location
    for (auto target = 0; target < Model::CONFIG->number_of_locations(); target++) {
      if (leavers[target] == 0) { continue; }
      perform_circulation_for_location(from, target, leavers[target],today_circulations);
    }
  }

  // Have the population do the movement
  for (auto* p : today_circulations) {
    p->randomly_choose_target_location();
  }
}

void PopulationMovement::perform_circulation_for_location(int from, int target, unsigned int count, std::vector<Person*>& today_circulations) {

  DoubleVector vLevelDensity;
  for (int i = 0; i < Model::CONFIG->circulation_info().number_of_moving_levels; i++) {
    auto temp = Model::CONFIG->circulation_info().v_moving_level_value[i] * static_cast<double>(_personIndex->vPerson()[from][i].size());
    vLevelDensity.push_back(temp);
  }

  std::vector<unsigned int> vIntNumberOfCirculation(vLevelDensity.size());
  Model::RANDOM->random_multinomial(static_cast<int>(vLevelDensity.size()), static_cast<unsigned int>(count), &vLevelDensity[0],&vIntNumberOfCirculation[0]);

  for (std::size_t moving_level = 0; moving_level < vIntNumberOfCirculation.size(); moving_level++) {
    auto size = static_cast<int>(_personIndex->vPerson()[from][moving_level].size());
    if (size==0) { continue; }

    for (std::size_t j = 0u; j < vIntNumberOfCirculation[moving_level]; j++) {
      //select 1 random person from level i
      auto index = Model::RANDOM->random_uniform(size);
      Person* p = _personIndex->vPerson()[from][moving_level][index];
      assert(p->host_state()!=Person::DEAD);
      p->today_target_locations()->push_back(target);
      today_circulations.push_back(p);
    }
  }
}
