/*
 *
 */
#include "PopulationMovement.h"

#include "Core/Config/Config.h"
#include "Core/ParallelJobs.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Spatial/SpatialModel.hxx"

std::vector<PopulationMovement::Circulation> PopulationMovement::perform_circulation_parallel(Population* population) {
#ifndef PARALLEL
  throw std::runtime_error("Parallel code is not enabled.");
#endif

  for (auto ndx = 0; ndx < Model::CONFIG->number_of_locations(); ndx++) {
    // How much of the population is moving? If none then press on
    auto poisson_means = static_cast<double>(population->size(ndx)) * Model::CONFIG->circulation_info().circulation_percent;
    if (poisson_means == 0) { continue; }
    const auto number_of_circulating_from_this_location = Model::RANDOM->random_poisson(poisson_means);
    if (number_of_circulating_from_this_location == 0) { continue; }

    // Perform the remainder of the circulation for this cell in parallel
    ParallelJobs::get_instance().submit(perform_circulation_location, ndx, number_of_circulating_from_this_location, this);
  }

  // Wait for everything to catch up before continuing
  while (ParallelJobs::get_instance().work_pending());

  // Return the movements
  return movements;
}

void PopulationMovement::perform_circulation_location(int from, int count, PopulationMovement* movement) {

  // Determine the likelihood of travel from the current location
  DoubleVector outMovement(Model::CONFIG->number_of_locations(), 0);
  outMovement = Model::CONFIG->spatial_model()->get_v_relative_out_movement_to_destination(from,
    Model::CONFIG->number_of_locations(), Model::CONFIG->spatial_distance_matrix()[from],
    Model::DATA_COLLECTOR->popsize_residence_by_location());

  // Determine the number of individuals that are leaving each location
  std::vector<unsigned int> leavers(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));
  Model::RANDOM->random_multinomial(static_cast<int>(outMovement.size()), static_cast<unsigned int>(count),&outMovement[0], &leavers[0]);

  // Generate the circulation events for each location
  PersonPtrVector today_circulations;
  for (std::size_t target_location = 0; target_location < Model::CONFIG->number_of_locations(); target_location++) {
    if (leavers[target_location] == 0) { continue; }
    movement->add_circulation(Circulation(from, static_cast<int>(target_location), static_cast<int>(leavers[target_location])));
  }

}

void PopulationMovement::add_circulation(PopulationMovement::Circulation circulation) {
  std::scoped_lock lock(circulation_mutex);
  movements.emplace_back(circulation);
}