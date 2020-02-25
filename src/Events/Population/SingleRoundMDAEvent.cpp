#define NOMINMAX

#include "SingleRoundMDAEvent.h"
#include "easylogging++.h"
#include "Model.h"
#include "date/date.h"
#include "Core/Config/Config.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Core/Random.h"
#include "Events/ReceiveMDATherapyEvent.h"

SingleRoundMDAEvent::SingleRoundMDAEvent(const int &execute_at) {
  time = execute_at;
}

void SingleRoundMDAEvent::execute() {
  LOG(INFO) << date::year_month_day{scheduler->calendar_date} << ": executing Single Round MDA";


  // for all location
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    // step 1: get number of individuals for MDA
    auto pi_lsa = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    std::vector<Person *> all_persons_in_location;
    for (auto hs = 0; hs < Person::DEAD; hs++) {
      for (std::size_t ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto p : pi_lsa->vPerson()[loc][hs][ac]) {
          all_persons_in_location.push_back(p);
        }
      }
    }

    const std::size_t number_indidividuals_in_location = all_persons_in_location.size();
    unsigned int number_of_individuals_will_receive_mda = Model::RANDOM->random_poisson(fraction_population_targeted[loc]*number_indidividuals_in_location);

    number_of_individuals_will_receive_mda = number_of_individuals_will_receive_mda > number_indidividuals_in_location
        ? number_indidividuals_in_location : number_of_individuals_will_receive_mda;
    //shuffle app_persons_in_location index for sampling without replacement
    Model::RANDOM->shuffle(&all_persons_in_location[0], all_persons_in_location.size(), sizeof(size_t));

    for (std::size_t p_i = 0; p_i < number_of_individuals_will_receive_mda; p_i++) {
      auto p = all_persons_in_location[p_i];
      //step 2: determine whether person will receive treatment
      const auto prob = Model::RANDOM->random_flat(0.0, 1.0);
      if (prob < p->prob_present_at_mda()) {
        // receive MDA
        auto *therapy = Model::CONFIG->therapy_db()[Model::CONFIG->mda_therapy_id()];
        // schedule received therapy in within days_to_complete_all_treatments
        int days_to_receive_mda_therapy = Model::RANDOM->random_uniform(days_to_complete_all_treatments) + 1;
        ReceiveMDATherapyEvent::schedule_event(Model::SCHEDULER, p, therapy,
                                               Model::SCHEDULER->current_time() +
                                                   days_to_complete_all_treatments);

      }
    }
  }

}
