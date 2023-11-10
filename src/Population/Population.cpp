/*
 * Population.cpp
 *
 * Implement the population data structure.
 */
#include "Population.h"

#include <cfloat>
#include <cmath>

#include "Constants.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "easylogging++.h"
#include "Events/BirthdayEvent.h"
#include "Events/RaptEvent.h"
#include "Events/SwitchImmuneComponentEvent.h"
#include "Helpers/ObjectHelpers.h"
#include "Helpers/TimeHelpers.h"
#include "ImmuneSystem.h"
#include "InfantImmuneComponent.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "NonInfantImmuneComponent.h"
#include "Properties/PersonIndexAll.h"
#include "Properties/PersonIndexByLocationBittingLevel.h"
#include "Properties/PersonIndexByLocationMovingLevel.h"
#include "Properties/PersonIndexByLocationStateAgeClass.h"
#include "Spatial/SpatialModel.hxx"

Population::Population(Model* model) : model_(model) {
  person_index_list_ = new PersonIndexPtrList();
  all_persons_ = new PersonIndexAll();
  person_index_list_->push_back(all_persons_);
}

Population::~Population() {
  //release memory for all persons
  if (all_persons_!=nullptr) {
    for (auto &person : all_persons_->vPerson()) {
      ObjectHelpers::delete_pointer<Person>(person);
    }
    all_persons_->vPerson().clear();
    all_persons_ = nullptr;
  }

  //release person_indexes

  if (person_index_list_!=nullptr) {

    for (PersonIndex* person_index : *person_index_list_) {
      ObjectHelpers::delete_pointer<PersonIndex>(person_index);
    }

    person_index_list_->clear();
    ObjectHelpers::delete_pointer<PersonIndexPtrList>(person_index_list_);
  }
}

void Population::add_person(Person* person) {
  for (PersonIndex* person_index : *person_index_list_) {
    person_index->add(person);
  }
  person->set_population(this);

  if (person->all_clonal_parasite_populations()->size() > 0) {
    person->all_clonal_parasite_populations()->add_all_infection_force();
  }

  // Update the count at the location
  popsize_by_location_[person->location()]++;
}

void Population::remove_person(Person* person) {
  // If they are already dead, remove the infection to prevent double counting
  if (person->host_state()!=Person::DEAD) {
    person->all_clonal_parasite_populations()->remove_all_infection_force();
  }

  for (PersonIndex* person_index : *person_index_list_) {
    person_index->remove(person);
  }
  person->set_population(nullptr);

  // Update the count at the location
  popsize_by_location_[person->location()]--;
  assert(popsize_by_location_[person->location()] >= 0);
}

void Population::remove_dead_person(Person* person) {
  remove_person(person);
  ObjectHelpers::delete_pointer<Person>(person);
}

void Population::notify_change(Person* p, const Person::Property &property, const void* oldValue,
                               const void* newValue) {

  for (PersonIndex* person_index : *person_index_list_) {
    person_index->notify_change(p, property, oldValue, newValue);
  }
}

void Population::notify_movement(const int source, const int destination) {
  popsize_by_location_[source]--;
  assert(popsize_by_location_[source] >= 0);
  popsize_by_location_[destination]++;
}

std::size_t Population::size() {
  return all_persons_->size();
}

std::size_t Population::size(const int &location) {
  return popsize_by_location_[location];
}

void Population::perform_infection_event() {
  PersonPtrVector today_infections;
  
#ifdef DEBUG
  auto start = std::chrono::system_clock::now();
#endif

  // Get the person index
  auto pi = get_person_index<PersonIndexByLocationBittingLevel>();

  // Iterate over all the locations in the model
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {

    // Calculate location adjustments
    
    const auto new_beta = Model::CONFIG->location_db()[loc].beta * Model::CONFIG->seasonal_info()->get_seasonal_factor(Model::SCHEDULER->calendar_date, loc);

    // Iterate over all the parasite types
    for (std::size_t parasite_type_id = 0; parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {

      // Calculate the force of infection if ~0 then press on 
      auto trackingDays = Model::SCHEDULER->current_time()%Model::CONFIG->number_of_tracking_days();
      const auto force_of_infection = force_of_infection_for7days_by_location_parasite_type_[trackingDays][loc][parasite_type_id];
      if (force_of_infection <= DBL_EPSILON) { continue; }

      // Calculate the number of bites, if 0 then press on
      auto poisson_means = new_beta*force_of_infection;
      auto number_of_bites = Model::RANDOM->random_poisson(poisson_means);
      if (number_of_bites <= 0) { continue; }

      //data_collector store number of bites
      Model::DATA_COLLECTOR->collect_number_of_bites(loc, number_of_bites);

      // Determine the distribution of the bites
      DoubleVector vLevelDensity;
      for (int i = 0; i < Model::CONFIG->relative_bitting_info().number_of_biting_levels; i++) {
        auto temp = Model::CONFIG->relative_bitting_info().v_biting_level_value[i] * static_cast<double>(pi->vPerson()[loc][i].size());
        vLevelDensity.push_back(temp);
      }
      std::vector<unsigned int> v_int_number_of_bites(vLevelDensity.size());
      model_->random()->random_multinomial(vLevelDensity.size(), number_of_bites, &vLevelDensity[0], &v_int_number_of_bites[0]);

      // Inflict the bites upon the population
      for (std::size_t biting_level = 0; biting_level < v_int_number_of_bites.size(); biting_level++) {
        // If there is nobody at this level, press on
        const auto size = pi->vPerson()[loc][biting_level].size();
        if (size==0) { continue; }

        for (std::size_t j = 0u; j < v_int_number_of_bites[biting_level]; j++) {
          //select 1 random person from level i
          const auto index = model_->random()->random_uniform(size);
          auto* person = pi->vPerson()[loc][biting_level][index];

          // If the person is not dead, inflict the bite upon them,
          // an update today's infection if they get infected
          assert(person->host_state()!=Person::DEAD);
          if (person->inflict_bite(parasite_type_id)) {
            today_infections.push_back(person);
          }
        }
      }
    }
  }

  // TODO solve Multiple infections
  if (today_infections.empty()) return;

  for (auto* p : today_infections) {
    if (!p->today_infections()->empty()) {
      Model::DATA_COLLECTOR->record_1_infection(p->location());
    }
    p->randomly_choose_parasite();
  }

#ifdef DEBUG
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  if (elapsed_seconds.count() >= 1) {
    LOG(INFO) << "Long infection event" << elapsed_seconds.count();
  }
#endif
}

void Population::initialize() {
  // Return if there is no model
  if (model() == nullptr) { return; }

  // Prepare the population size vector
  popsize_by_location_ = IntVector(Model::CONFIG->number_of_locations(), 0);

  // Prepare the various mappings
  const auto number_of_location = Model::CONFIG->number_of_locations();
  const auto number_of_parasite_type = Model::CONFIG->number_of_parasite_types();

  current_force_of_infection_by_location_parasite_type_ = 
    DoubleVector2(number_of_location, DoubleVector(number_of_parasite_type, 0));

  interupted_feeding_force_of_infection_by_location_parasite_type_ = 
    DoubleVector2(number_of_location, DoubleVector(number_of_parasite_type, 0));

  force_of_infection_for7days_by_location_parasite_type_ = 
    std::vector<DoubleVector2>(Model::CONFIG->number_of_tracking_days(), DoubleVector2(number_of_location, DoubleVector(number_of_parasite_type, 0)));

  // Initialize other person index
  initialize_person_indices();

  // Initialize population
  auto& location_db = Model::CONFIG->location_db();
  for (auto loc = 0; loc < number_of_location; loc++) {
    VLOG(9) << fmt::format("Cell {}, population {}", loc, location_db[loc].population_size);

    const auto popsize_by_location = static_cast<int>(location_db[loc].population_size * Model::CONFIG->artificial_rescaling_of_population_size());
    auto temp_sum = 0;
    for (auto age_class = 0; age_class < Model::CONFIG->initial_age_structure().size(); age_class++) {
      auto number_of_individual_by_loc_ageclass = 0;
      if (age_class == Model::CONFIG->initial_age_structure().size() - 1) {
        number_of_individual_by_loc_ageclass = popsize_by_location - temp_sum;
      } else {
        number_of_individual_by_loc_ageclass = static_cast<int>(popsize_by_location * location_db[loc].age_distribution[age_class]);
        temp_sum += number_of_individual_by_loc_ageclass;
      }
      for (int i = 0; i < number_of_individual_by_loc_ageclass; i++) {
        generate_individual(loc, age_class);
      }
    }
  }
}

void Population::generate_individual(int location, int age_class) {
  auto p = new Person();
  p->init();

  p->set_location(location);
  p->set_residence_location(location);
  p->set_host_state(Person::SUSCEPTIBLE);

  // Set the age of the individual, which also sets the age class. Note that we are defining the types to conform to the
  // signature of random_uniform_int
  unsigned long age_from = (age_class==0) ? 0 : Model::CONFIG->initial_age_structure()[age_class - 1];
  unsigned long age_to = Model::CONFIG->initial_age_structure()[age_class];
  p->set_age(static_cast<int>(Model::RANDOM->random_uniform_int(age_from, age_to + 1)));

  auto days_to_next_birthday = static_cast<int>(Model::RANDOM->random_uniform(Constants::DAYS_IN_YEAR()));
  auto simulation_time_birthday = TimeHelpers::get_simulation_time_birthday(days_to_next_birthday, p->age(), Model::SCHEDULER->calendar_date);
  p->set_birthday(simulation_time_birthday);

  LOG_IF(simulation_time_birthday > 0, FATAL) << "simulation_time_birthday have to be <= 0 when initializing population";

  BirthdayEvent::schedule_event(Model::SCHEDULER, p, days_to_next_birthday);
  RaptEvent::schedule_event(Model::SCHEDULER, p, days_to_next_birthday);

  //set immune component
  if (simulation_time_birthday + Constants::DAYS_IN_YEAR()/2 >= 0) {
    LOG_IF(p->age() > 0, FATAL) << "Error in calculating simulation_time_birthday";
    p->immune_system()->set_immune_component(new InfantImmuneComponent());
    //schedule for switch
    auto time = simulation_time_birthday + Constants::DAYS_IN_YEAR() / 2;
    SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Model::SCHEDULER, p, time);
  } else {
    p->immune_system()->set_immune_component(new NonInfantImmuneComponent());
  }

  auto immune_value = Model::RANDOM->random_beta(
      Model::CONFIG->immune_system_information().alpha_immune,
      Model::CONFIG->immune_system_information().beta_immune);
  p->immune_system()->immune_component()->set_latest_value(immune_value);
  p->immune_system()->set_increase(false);

  p->set_bitting_level(Model::CONFIG->bitting_level_generator().draw_random_level(Model::RANDOM));
  p->set_base_bitting_level_value(Model::CONFIG->relative_bitting_info().v_biting_level_value[p->bitting_level()]);

  p->set_moving_level(Model::CONFIG->moving_level_generator().draw_random_level(Model::RANDOM));

  p->set_latest_update_time(0);

  auto time = static_cast<int>(Model::RANDOM->random_uniform(Model::CONFIG->update_frequency()) + 1);
  p->schedule_update_every_K_days_event(time);
  p->generate_prob_present_at_mda_by_age();

  add_person(p);
}

void Population::introduce_initial_cases() {
  // Return if there is no model
  if (model() == nullptr) { return; }

  for (const auto p_info : Model::CONFIG->initial_parasite_info()) {
    // Make sure the parasite is in bounds
    if (p_info.parasite_type_id >= Model::CONFIG->genotype_db()->size()) {
      LOG(FATAL) << fmt::format("Unrecoverable error! Initial parasite id ({}) is greater or equal to the size of the genotype_db ({})", 
        p_info.parasite_type_id, Model::CONFIG->genotype_db()->size());
      exit(EXIT_FAILURE);
    }

    auto count = static_cast<double>(size(p_info.location));
    auto num_of_infections = Model::RANDOM->random_poisson(std::round(count * p_info.prevalence));
    num_of_infections = num_of_infections <= 0 ? 1 : num_of_infections;
    
    auto* genotype = Model::CONFIG->genotype_db()->at(p_info.parasite_type_id);
    VLOG(9) << fmt::format("Introducing genotype {}  with prevalence: {} : {} infections at location {}", 
      p_info.parasite_type_id, p_info.prevalence, num_of_infections,  p_info.location);
    introduce_parasite(p_info.location, genotype, num_of_infections);
  }

  //update force of infection for 7 days
  for (int d = 0; d < Model::CONFIG->number_of_tracking_days(); d++) {
    for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
      for (std::size_t genotype = 0; genotype < Model::CONFIG->number_of_parasite_types(); genotype++) {
        force_of_infection_for7days_by_location_parasite_type_[d][loc][genotype] =
            current_force_of_infection_by_location_parasite_type_[loc][genotype];
      }
    }
  }
}

void Population::introduce_parasite(const int &location, Genotype* parasite_type, const int &num_of_infections) {
  // Return if there is no model
  if (model() == nullptr) { return; }

  DoubleVector vLevelDensity;
  auto pi = get_person_index<PersonIndexByLocationBittingLevel>();

  for (int i = 0; i < Model::CONFIG->relative_bitting_info().number_of_biting_levels; i++) {
    auto temp = Model::CONFIG->relative_bitting_info().v_biting_level_value[i] * static_cast<double>(pi->vPerson()[location][i].size());
    vLevelDensity.push_back(temp);
  }

  std::vector<unsigned int> vIntNumberOfBites(vLevelDensity.size());
  model_->random()->random_multinomial(vLevelDensity.size(), num_of_infections, &vLevelDensity[0], &vIntNumberOfBites[0]);

  for (std::size_t biting_level = 0; biting_level < vIntNumberOfBites.size(); biting_level++) {
    auto size = pi->vPerson()[location][biting_level].size();
    if (size==0) continue;
    for (std::size_t j = 0u; j < vIntNumberOfBites[biting_level]; j++) {

      //select 1 random person from level i
      auto index = model_->random()->random_uniform(size);
      auto* p = pi->vPerson()[location][biting_level][index];

      initial_infection(p, parasite_type);
    }
  }
}

void Population::initial_infection(Person* person, Genotype* parasite_type) const {

  person->immune_system()->set_increase(true);
  person->set_host_state(Person::ASYMPTOMATIC);

  auto* blood_parasite = person->add_new_parasite_to_blood(parasite_type);

  const auto size = model_->random()->random_flat(
      Model::CONFIG->parasite_density_level().log_parasite_density_from_liver,
      Model::CONFIG->parasite_density_level().log_parasite_density_clinical);

  blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
  blood_parasite->set_last_update_log10_parasite_density(size);

  const auto p_clinical = person->get_probability_progress_to_clinical();
  const auto p = model_->random()->random_flat(0.0, 1.0);

  if (p < p_clinical) {
    //progress to clinical after several days
    blood_parasite->set_update_function(model_->progress_to_clinical_update_function());
    person->schedule_progress_to_clinical_event_by(blood_parasite);
  } else {
    //only progress to clearance by Immune system
    //progress to clearance
    blood_parasite->set_update_function(model_->immunity_clearance_update_function());
  }
}

void Population::notify_change_in_force_of_infection(const int &location, const int &parasite_type_id,
                                                     const double &relative_force_of_infection) {
  current_force_of_infection_by_location_parasite_type_[location][parasite_type_id] += relative_force_of_infection;
}

void Population::update_force_of_infection(const int &current_time) {
  perform_interrupted_feeding_recombination();

  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (std::size_t p_type = 0; p_type < Model::CONFIG->number_of_parasite_types(); p_type++) {
      force_of_infection_for7days_by_location_parasite_type()[current_time%
          Model::CONFIG->number_of_tracking_days()][loc][p_type] =
          interupted_feeding_force_of_infection_by_location_parasite_type()[loc][
              p_type];
    }
  }
}

// Free space in the population indicies.
void Population::update() {
  for (PersonIndex* person_index : *person_index_list_) {
    person_index->defragment();
  }
}

void Population::perform_birth_event() {
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    auto poisson_means = static_cast<double>(size(loc)) * Model::CONFIG->birth_rate() / Constants::DAYS_IN_YEAR();
    const auto number_of_births = Model::RANDOM->random_poisson(poisson_means);
    for (int i = 0; i < number_of_births; i++) {
      give_1_birth(loc);
      Model::DATA_COLLECTOR->record_1_birth(loc);
      Model::DATA_COLLECTOR->update_person_days_by_years(loc, Constants::DAYS_IN_YEAR() -
          Model::SCHEDULER->current_day_in_year());
    }
  }
}

void Population::give_1_birth(const int &location) {
  auto p = new Person();
  p->init();
  p->set_age(0);
  p->set_host_state(Person::SUSCEPTIBLE);
  p->set_age_class(0);
  p->set_location(location);
  p->set_residence_location(location);
  p->immune_system()->set_immune_component(new InfantImmuneComponent());
  p->immune_system()->set_latest_immune_value(1.0);
  p->immune_system()->set_increase(false);

  p->set_latest_update_time(Model::SCHEDULER->current_time());

  //set_relative_biting_rate
  p->set_bitting_level(Model::CONFIG->bitting_level_generator().draw_random_level(Model::RANDOM));
  p->set_base_bitting_level_value(
      Model::CONFIG->relative_bitting_info().v_biting_level_value[p->bitting_level()]);

  p->set_moving_level(Model::CONFIG->moving_level_generator().draw_random_level(Model::RANDOM));

  p->set_birthday(Model::SCHEDULER->current_time());
  const auto number_of_days_to_next_birthday = TimeHelpers::
  number_of_days_to_next_year(Model::SCHEDULER->calendar_date);
  BirthdayEvent::schedule_event(Model::SCHEDULER, p,
                                Model::SCHEDULER->current_time() + number_of_days_to_next_birthday);

  RaptEvent::schedule_event(
      Model::SCHEDULER, p,
      Model::SCHEDULER->current_time() + number_of_days_to_next_birthday);
  //schedule for switch
  SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Model::SCHEDULER, p,
                                                                         Model::SCHEDULER->current_time() +
                                                                             Constants::
                                                                             DAYS_IN_YEAR()/2);

  p->schedule_update_every_K_days_event(Model::CONFIG->update_frequency());
  p->generate_prob_present_at_mda_by_age();

  add_person(p);
}

// Scan the population and perform death events and clear the deceased persons from other parts of the model. 
void Population::perform_death_event() {

  // TODO Move this elsewhere
  assert(Model::CONFIG->death_rate_by_age_class().size()==Model::CONFIG->number_of_age_classes());

  // Get the person index and return if it is null
  auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
  if (pi == nullptr) { return; }

  PersonPtrVector deceased;

  // Iterate over the locations, age brackets, and states
  for (std::size_t loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    for (std::size_t ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
      for (std::size_t hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {

        // Skip the dead state for the outer scan
        if (hs == Person::DEAD) { 
          continue; 
        }
      
        // Press on if there is nothing to do
        auto size = pi->vPerson()[loc][hs][ac].size();
        if (size == 0) { continue; }

        // Determine the number of deaths, press on if there are none
        auto poisson_means = static_cast<double>(size) * Model::CONFIG->death_rate_by_age_class()[ac] / Constants::DAYS_IN_YEAR();
        const auto number_of_deaths = Model::RANDOM->random_poisson(poisson_means);
        if (number_of_deaths == 0) { continue; }

        // Change the state for the number of deaths based upon the actuarial likelihood
        for (int i = 0; i < number_of_deaths; i++) {
          auto index = Model::RANDOM->random_uniform(size);
          auto* p = pi->vPerson()[loc][hs][ac][index];
          p->cancel_all_events_except(nullptr);
          p->set_host_state(Person::DEAD);
        }
      }

      // Scan for any deceased people
      for (auto person : pi->vPerson()[loc][Person::DEAD][ac]) {
        deceased.push_back(person);
      }
    }
  }

  // Remove the deceased from the model
  for (auto person : deceased) {
    remove_dead_person(person);
  }
}

void Population::perform_circulation_event() {
#ifdef DEBUG
  auto start = std::chrono::system_clock::now();
#endif

  // Before entering, check the overall circulation, if zero then assume it is disabled
  auto circulation_percent = Model::CONFIG->circulation_info().circulation_percent;
  if (circulation_percent == 0.0) { return; }

  PersonPtrVector today_circulations;

  // Grab a copy of residents by location
  const auto residents_by_location = Model::DATA_COLLECTOR->popsize_residence_by_location();

  //for each location
  // get number of circulations based on size * circulation_percent
  // distributes that number into others location based of other location size
  // for each number in that list select an individual, and schedule a movement event on next day
  for (auto from_location = 0; from_location < Model::CONFIG->number_of_locations(); from_location++) {
    
    // How much of the population is moving? If none then press on
    auto poisson_means = static_cast<double>(size(from_location)) * circulation_percent;
    if (poisson_means == 0) { continue; }
    const auto number_of_circulating_from_this_location = Model::RANDOM->random_poisson(poisson_means);
    if (number_of_circulating_from_this_location == 0) { continue; }

    DoubleVector v_relative_outmovement_to_destination(Model::CONFIG->number_of_locations(), 0);
    v_relative_outmovement_to_destination = Model::CONFIG->spatial_model()->get_v_relative_out_movement_to_destination(
      from_location, static_cast<int>(Model::CONFIG->number_of_locations()), Model::CONFIG->spatial_distance_matrix()[from_location], residents_by_location);

    std::vector<unsigned int> v_num_leavers_to_destination(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));
    Model::RANDOM->random_multinomial(static_cast<int>(v_relative_outmovement_to_destination.size()),
                                      static_cast<unsigned int>(number_of_circulating_from_this_location),
                                      &v_relative_outmovement_to_destination[0], &v_num_leavers_to_destination[0]);

    for (auto target_location = 0; target_location < Model::CONFIG->number_of_locations(); target_location++) {
      if (v_num_leavers_to_destination[target_location]==0) { continue; }
      perform_circulation_for_1_location(from_location, target_location, static_cast<int>(v_num_leavers_to_destination[target_location]), today_circulations);
    }
  }

  // Have the population do the movement
  for (auto* p : today_circulations) {
    p->randomly_choose_target_location();
  }

#ifdef DEBUG
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  if (elapsed_seconds.count() >= 1) {
    LOG(INFO) << "Long circulation" << elapsed_seconds.count();
  }
#endif
}

void Population::perform_circulation_for_1_location(const int &from_location, const int &target_location,
                                                    const int &number_of_circulation,
                                                    std::vector<Person*> &today_circulations) {
  DoubleVector vLevelDensity;
  auto pi = get_person_index<PersonIndexByLocationMovingLevel>();

  for (int i = 0; i < Model::CONFIG->circulation_info().number_of_moving_levels; i++) {
    auto temp = Model::CONFIG->circulation_info().v_moving_level_value[i] * static_cast<double>(pi->vPerson()[from_location][i].size());
    vLevelDensity.push_back(temp);
  }

  std::vector<unsigned int> vIntNumberOfCirculation(vLevelDensity.size());

  model_->random()->random_multinomial(static_cast<int>(vLevelDensity.size()),
                                       static_cast<unsigned int>(number_of_circulation), &vLevelDensity[0],
                                       &vIntNumberOfCirculation[0]);

  for (std::size_t moving_level = 0; moving_level < vIntNumberOfCirculation.size(); moving_level++) {
    auto size = static_cast<int>(pi->vPerson()[from_location][moving_level].size());
    if (size==0) continue;
    for (std::size_t j = 0u; j < vIntNumberOfCirculation[moving_level]; j++) {


      //select 1 random person from level i
      auto index = model_->random()->random_uniform(size);
      Person* p = pi->vPerson()[from_location][moving_level][index];
      assert(p->host_state()!=Person::DEAD);

      p->today_target_locations()->push_back(target_location);
      today_circulations.push_back(p);

    }
  }
}

void Population::initialize_person_indices() {
  auto number_of_location = static_cast<int>(Model::CONFIG->number_of_locations());
  auto number_of_host_states = Person::NUMBER_OF_STATE;
  auto number_of_age_classes = static_cast<int>(Model::CONFIG->number_of_age_classes());

  auto p_index_by_l_s_a = new PersonIndexByLocationStateAgeClass(
          number_of_location, number_of_host_states, number_of_age_classes);
  person_index_list_->push_back(p_index_by_l_s_a);

  auto p_index_location_biting_level = new PersonIndexByLocationBittingLevel(
      number_of_location, Model::CONFIG->relative_bitting_info().number_of_biting_levels);
  person_index_list_->push_back(p_index_location_biting_level);

  auto p_index_location_moving_level = new PersonIndexByLocationMovingLevel(
      number_of_location, Model::CONFIG->circulation_info().number_of_moving_levels);
  person_index_list_->push_back(p_index_location_moving_level);
}

void Population::perform_interrupted_feeding_recombination() {
  // Cache some values
  auto parasite_types = Model::CONFIG->number_of_parasite_types();
  auto number_of_locations = Model::CONFIG->number_of_locations();

  // calculate vector Y, Z
  auto y = DoubleVector2(static_cast<unsigned long long int>(number_of_locations), DoubleVector(parasite_types, 0));
  auto z = DoubleVector2(static_cast<unsigned long long int>(number_of_locations), DoubleVector(parasite_types, 0));

  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    for (std::size_t parasite_type_id = 0; parasite_type_id < parasite_types; parasite_type_id++) {
      interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] =
          current_force_of_infection_by_location_parasite_type_[loc][parasite_type_id];
      y[loc][parasite_type_id] =
          interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] * (1 - Model::CONFIG->fraction_mosquitoes_interrupted_feeding());
      z[loc][parasite_type_id] =
          interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] * Model::CONFIG->fraction_mosquitoes_interrupted_feeding();
    }
  }

  auto number_of_gametocytaemic = 0;

  auto pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    //hs 2: asymptomatic, 3: clinical
    for (std::size_t hs = 2; hs <= 3; hs++) {
      for (std::size_t ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        for (auto p : pi->vPerson()[loc][hs][ac]) {
          if (p->isGametocytaemic()) {
            number_of_gametocytaemic++;
          }
        }
      }
    }
  }

  // Find the sum, use it to calculate a
  double sum_z = 0;
  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    for (std::size_t parasite_type_id = 0; parasite_type_id < parasite_types; parasite_type_id++) {
      sum_z += z[loc][parasite_type_id];
    }
  }
  const auto a = Model::CONFIG->fraction_mosquitoes_interrupted_feeding() * number_of_gametocytaemic / sum_z;

  // Calculate the new z value and the new sum
  sum_z = 0;
  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    for (std::size_t parasite_type_id = 0; parasite_type_id < parasite_types; parasite_type_id++) {
      z[loc][parasite_type_id] = static_cast<double>(std::lround(a * z[loc][parasite_type_id]));
      sum_z += z[loc][parasite_type_id];
    }
  }

  // sum_z = 0;
  // for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
  //   for (auto parasite_type_id = 0; parasite_type_id < parasite_types; parasite_type_id++) {
  //     sum_z += z[loc][parasite_type_id];
  //   }
  // }
  

  //expected allele frequencies after recombination
  auto eafar = DoubleVector2(static_cast<unsigned long long int>(number_of_locations), DoubleVector(parasite_types, 0));

  // perform free recombination in Z
  double sum_eafar = 0;
  if (sum_z > 0.0001) {
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      for (auto i = 0; i < parasite_types; i++) {
        if (z[loc][i]==0) continue;
        for (auto j = 0; j < parasite_types; j++) {
          if (z[loc][j]==0) continue;
          if (i==j) {
            const auto weight = z[loc][i]*z[loc][i];
            eafar[loc][i] += weight;
          } else {
            const auto weight = 2*z[loc][i]*z[loc][j];
            for (auto p = 0; p < parasite_types; p++) {
              if (Model::CONFIG->genotype_db()->get_offspring_density(i, j, p)==0) continue;
              eafar[loc][p] += weight*Model::CONFIG->genotype_db()->get_offspring_density(i, j, p);
            }
          }
        }
      }
    }

    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      for (std::size_t i = 0; i < parasite_types; i++) {
        eafar[loc][i] /= (sum_z*sum_z);
        sum_eafar += eafar[loc][i];
      }
    }

    //normalize eafar
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      for (std::size_t i = 0; i < parasite_types; i++) {
        eafar[loc][i] /= sum_eafar;
      }
    }

    //weight Z with eafar and divide by a and calculate current_force_of_infection
    auto new_z = std::vector<unsigned int>(parasite_types, 0);
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      
      Model::RANDOM->random_multinomial(static_cast<const int &>(eafar[loc].size()), static_cast<const unsigned int &>(sum_z), &eafar[loc][0], &new_z[0]);

      for (std::size_t parasite_type_id = 0; parasite_type_id < parasite_types; parasite_type_id++) {
        z[loc][parasite_type_id] = new_z[parasite_type_id]/a;
        interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] = y[loc][parasite_type_id] + z[loc][parasite_type_id];

        // This new_z value has been used, so clear it
        new_z[parasite_type_id] = 0;
      }
    }
  }
}
