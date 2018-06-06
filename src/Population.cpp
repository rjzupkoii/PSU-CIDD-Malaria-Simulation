/* 
 * File:   Population.cpp
 * Author: nguyentran
 * 
 * Created on April 15, 2013, 10:49 AM
 */

#include "Population.h"
#include "Model.h"
#include "PersonIndexAll.h"
#include "HelperFunction.h"
#include "Config.h"
#include "PersonIndexByLocationStateAgeClass.h"
#include "InfantImmuneComponent.h"
#include "NonInfantImmuneComponent.h"
#include "ImmuneSystem.h"
#include "SwitchImmuneComponentEvent.h"
#include "BirthdayEvent.h"
#include "PersonIndexByLocationBittingLevel.h"
#include "Random.h"
#include "PersonIndexByLocationMovingLevel.h"
#include "MDC/ModelDataCollector.h"
#include "SingleHostClonalParasitePopulations.h"
#include "PersonIndexByLocationExternalPopulationMovingLevel.h"
#include "MoveToExternalPopulationEvent.h"

Population::Population(Model *model) : model_(model) {
    person_index_list_ = new PersonIndexPtrList();
    all_persons_ = new PersonIndexAll();

    person_index_list_->push_back(all_persons_);
}

Population::~Population() {
    //release memory for all persons
    if (all_persons_ != nullptr) {
        for (auto &person : all_persons_->vPerson()) {
            DeletePointer<Person>(person);
        }
        all_persons_->vPerson().clear();
        all_persons_ = nullptr;
    }

    //release person_indexes

    if (person_index_list_ != nullptr) {

        for (PersonIndex *person_index: *person_index_list_) {
            DeletePointer<PersonIndex>(person_index);
        }

        person_index_list_->clear();
        DeletePointer<PersonIndexPtrList>(person_index_list_);
    }
}

void Population::add_person(Person *person) {

    for (PersonIndex *person_index: *person_index_list_) {
        person_index->add(person);
    }
    person->set_population(this);

    if (person->all_clonal_parasite_populations()->size() > 0) {
        person->all_clonal_parasite_populations()->add_all_infection_force();
    }
}

void Population::remove_person(Person *person) {
    if (person->host_state() != Person::DEAD) {
        person->all_clonal_parasite_populations()->remove_all_infection_force();
    }

    for (PersonIndex *person_index: *person_index_list_) {
        person_index->remove(person);
    }
}

void Population::remove_dead_person(Person *person) {
    remove_person(person);
    DeletePointer<Person>(person);
}

void Population::notify_change(Person *p, const Person::PersonProperties &property, const void *oldValue,
                               const void *newValue) {

    for (PersonIndex *person_index: *person_index_list_) {
        person_index->notify_change(p, property, oldValue, newValue);
    }
}

int Population::size(const int &location, const int &age_class) {
    if (location == -1) {
        return all_persons_->size();
    } else {
        auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();

        if (pi_lsa == nullptr) {
            return 0;
        }
        int temp = 0;
        if (age_class == -1) {

            for (int state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
                for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                    temp += pi_lsa->vPerson()[location][state][ac].size();
                }
            }
        } else {
            for (int state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
                temp += pi_lsa->vPerson()[location][state][age_class].size();
            }
        }
        return temp;

    }
}

int Population::size(const int &location, const Person::HostStates &hs, const int &age_class) {
    if (location == -1) {
        return all_persons_->size();
    } else {
        auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();
        return static_cast<int>(pi_lsa->vPerson()[location][hs][age_class].size());
    }
}


//new
int Population::size_residents_only(const int &location) {
    if (location == -1) {
        return all_persons_->size();
    }

    auto pi_lsa = get_person_index<PersonIndexByLocationStateAgeClass>();

    if (pi_lsa == nullptr) {
        return 0;
    }
    int temp = 0;
    for (int state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
            for (int i = 0; i < pi_lsa->vPerson()[location][state][ac].size(); i++) {
                if (pi_lsa->vPerson()[location][state][ac][i]->residence_location() == location) {
                    temp++;
                }
            }
        }
    }
    return temp;
}

void Population::perform_infection_event() {
    //    std::cout << "Infection Event" << std::endl;

    PersonPtrVector today_infections;
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int parasite_type_id = 0;
             parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
            double force_of_infection = force_of_infection_for7days_by_location_parasite_type_[
                    Model::SCHEDULER->current_time() % Model::CONFIG->number_of_tracking_days()][loc][parasite_type_id];
            if (force_of_infection <= 0.000001)
                continue;

            double newBeta = Model::CONFIG->location_db()[loc].beta *
                    Model::CONFIG->seasonal_factor_for_beta(Model::SCHEDULER->current_time());

            double poisson_means = newBeta * force_of_infection;

            int number_of_bites = Model::RANDOM->random_poisson(poisson_means);
            if (number_of_bites <= 0)
                continue;

            //data_collector store number of bites
            Model::DATA_COLLECTOR->collect_number_of_bites(loc, number_of_bites);

            //             if (Global::scheduler->currentTime >= Global::startTreatmentDay) {
            //                //scale down to 1000
            //                Report::TotalNumberOfBites += numberOfInfections;
            //                Report::TotalNumberOfBitesByYear += numberOfInfections;
            //            }

            DoubleVector vLevelDensity;
            auto pi = get_person_index<PersonIndexByLocationBittingLevel>();

            for (int i = 0; i < Model::CONFIG->relative_bitting_information().number_of_biting_levels; i++) {
                double temp = Model::CONFIG->relative_bitting_information().v_biting_level_value[i] *
                              pi->vPerson()[loc][i].size();
                vLevelDensity.push_back(temp);
            }

            std::vector<unsigned int> vIntNumberOfBites(vLevelDensity.size());
            model_->random()->random_multinomial(vLevelDensity.size(), number_of_bites, &vLevelDensity[0],
                                                 &vIntNumberOfBites[0]);


            for (int bitting_level = 0; bitting_level < vIntNumberOfBites.size(); bitting_level++) {
                int size = pi->vPerson()[loc][bitting_level].size();
                if (size == 0) continue;
                for (int j = 0; j < vIntNumberOfBites[bitting_level]; j++) {


                    //select 1 random person from level i
                    int index = model_->random()->random_uniform(size);
                    Person *p = pi->vPerson()[loc][bitting_level][index];

                    assert(p->host_state() != Person::DEAD);
                    p->increase_number_of_times_bitten();

                    double pInfectious = Model::RANDOM->random_flat(0.0, 1.0);
                    //only infect with real infectious bite
                    if (Model::CONFIG->using_variable_probability_infectious_bites_cause_infection()) {
                        if (pInfectious <= p->p_infection_from_an_infectious_bite()) {
                            if (p->host_state() != Person::EXPOSED && p->liver_parasite_type() == nullptr) {
                                p->today_infections()->push_back(parasite_type_id);
                                today_infections.push_back(p);
                            }
                        }
                    } else if (pInfectious <= Model::CONFIG->p_infection_from_an_infectious_bite()) {
                        if (p->host_state() != Person::EXPOSED && p->liver_parasite_type() == nullptr) {
                            p->today_infections()->push_back(parasite_type_id);
                            today_infections.push_back(p);
                        }
                    }

                }
            }
        }
    }
    //    std::cout << "Solve infections"<< std::endl;
    //solve Multiple infections
    if (today_infections.size() == 0) return;

    for (Person *p: today_infections) {
        p->randomly_choose_parasite();
    }

    today_infections.clear();

    //    std::cout << "End Infection Event" << std::endl;
}

void Population::initialize() {

    if (model() != nullptr) {
        // get population size, number of location, age_distribution from Model::CONFIG
        //        Config* Model::CONFIG = Model::CONFIG;


        int number_of_location = Model::CONFIG->number_of_locations();

        int number_of_parasite_type = Model::CONFIG->number_of_parasite_types();

        current_force_of_infection_by_location_parasite_type_ =
                DoubleVector2(number_of_location,
                              DoubleVector(
                                      number_of_parasite_type,
                                      0));
        interupted_feeding_force_of_infection_by_location_parasite_type_ = DoubleVector2(
                number_of_location, DoubleVector(number_of_parasite_type, 0));

        force_of_infection_for7days_by_location_parasite_type_ =
                std::vector<DoubleVector2>(
                        Model::CONFIG->number_of_tracking_days(),
                        DoubleVector2
                                (number_of_location, DoubleVector(number_of_parasite_type, 0)));

        //initalize other person index
        initialize_person_indices();

        //initialize population
        for (int loc = 0; loc < number_of_location; loc++) {
            int popsize_by_location = Model::CONFIG->location_db()[loc].populationSize;

            int temp_sum = 0;
            for (int age_class = 0; age_class < Model::CONFIG->initial_age_structure().size(); age_class++) {
                int number_of_individual_by_loc_ageclass = 0;
                if (age_class == Model::CONFIG->initial_age_structure().size() - 1) {
                    number_of_individual_by_loc_ageclass = popsize_by_location - temp_sum;
                } else {
                    number_of_individual_by_loc_ageclass =
                            popsize_by_location * Model::CONFIG->location_db()[loc].age_distribution[age_class];
                    temp_sum += number_of_individual_by_loc_ageclass;
                }

                //                std::cout << loc << "\t" << age_class << "\t" << number_of_individual_by_loc_ageclass << std::endl;
                for (int i = 0; i < number_of_individual_by_loc_ageclass; i++) {
                    auto p = new Person();
                    p->init();

                    p->set_location(loc);
                    p->set_residence_location(loc);
                    p->set_host_state(Person::SUSCEPTIBLE);

                    int age_from = (age_class == 0) ? 0 : Model::CONFIG->initial_age_structure()[age_class - 1];
                    int age_to = Model::CONFIG->initial_age_structure()[age_class];

                    //                    std::cout << i << "\t" << age_class << "\t" << age_from << "\t" << age_to << std::endl;

                    //set age will also set ageclass
                    p->set_age(Model::RANDOM->random_uniform_int(age_from, age_to + 1));
                    //                    std::cout << p->age() << " \t" << p->age_class() << std::endl;
                    //                    p->set_age_class(age_class);
                    p->set_birthday(-Model::RANDOM->random_uniform(360));

                    BirthdayEvent::schedule_event(Model::SCHEDULER, p, p->birthday() + 360);

                    //set immune component
                    if (p->is_infant(0)) {
                        p->immune_system()->set_immune_component(new InfantImmuneComponent());
                        //schedule for switch
                        SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Model::SCHEDULER, p,
                                                                                               p->birthday() + 360 / 2);
                    } else {
                        p->immune_system()->set_immune_component(new NonInfantImmuneComponent());
                    }

                    double immune_value = Model::RANDOM->random_beta(
                            Model::CONFIG->immune_system_information().alpha_immune,
                            Model::CONFIG->immune_system_information().beta_immune);
                    p->immune_system()->immune_component()->set_latest_value(immune_value);
                    p->immune_system()->set_increase(false);
                    //                    p->draw_random_immune();

                    //set_relative_biting_rate
                    p->set_bitting_level(Model::RANDOM->random_biting_level());
                    p->set_base_bitting_level_value(
                            Model::CONFIG->relative_bitting_information().v_biting_level_value[p->bitting_level()]);
                    //                    p->update_bitting_level();


                    p->set_moving_level(Model::RANDOM->random_moving_level());
                    p->set_external_population_moving_level(Model::RANDOM->random_external_population_moving_level());

                    p->set_latest_update_time(0);

                    int time = Model::RANDOM->random_uniform(Model::CONFIG->update_frequency()) + 1;
                    p->schedule_update_every_K_days_event(time);

                    add_person(p);
                }
            }
        }
    }
    //TODO: throw exception if model is nullptr
}

void Population::introduce_initial_cases() {
    if (model_ != nullptr) {

        for (InitialParasiteInfo p_info: Model::CONFIG->initial_parasite_info()) {
            int num_of_infections = size(p_info.location) * p_info.prevalence;
            //            std::cout << num_of_infections << std::endl;
            IntGenotype *genotype = Model::CONFIG->genotype_db()->db()[p_info.parasite_type_id];
            //std::cout << p_info.location << "-" << p_info.parasite_type_id << "-" << num_of_infections << std::endl;
            introduce_parasite(p_info.location, genotype, num_of_infections);

        }
        //update force of infection for 7 days
        for (int d = 0; d < Model::CONFIG->number_of_tracking_days(); d++) {
            for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
                for (int ptype = 0; ptype < Model::CONFIG->number_of_parasite_types(); ptype++) {
                    force_of_infection_for7days_by_location_parasite_type_[d][loc][ptype] = current_force_of_infection_by_location_parasite_type_[loc][ptype];
                }
            }
        }
    }
}

void Population::introduce_parasite(const int &location, IntGenotype *parasite_type, const int &num_of_infections) {

    if (model_ != nullptr) {

        DoubleVector vLevelDensity;
        auto pi = get_person_index<PersonIndexByLocationBittingLevel>();

        for (int i = 0; i < Model::CONFIG->relative_bitting_information().number_of_biting_levels; i++) {
            double temp = Model::CONFIG->relative_bitting_information().v_biting_level_value[i] *
                          pi->vPerson()[location][i].size();
            vLevelDensity.push_back(temp);
        }

        std::vector<unsigned int> vIntNumberOfBites(vLevelDensity.size());
        model_->random()->random_multinomial(vLevelDensity.size(), num_of_infections, &vLevelDensity[0],
                                             &vIntNumberOfBites[0]);

        for (int bitting_level = 0; bitting_level < vIntNumberOfBites.size(); bitting_level++) {
            int size = pi->vPerson()[location][bitting_level].size();
            if (size == 0) continue;
            for (int j = 0; j < vIntNumberOfBites[bitting_level]; j++) {

                //                std::cout << vIntNumberOfBites[bitting_level] << "-" << j << std::endl;
                //select 1 random person from level i
                int index = model_->random()->random_uniform(size);
                Person *p = pi->vPerson()[location][bitting_level][index];

                initial_infection(p, parasite_type);
            }
        }
    }
}

void Population::initial_infection(Person *p, IntGenotype *parasite_type) {

    p->immune_system()->set_increase(true);
    p->set_host_state(Person::ASYMPTOMATIC);

    ClonalParasitePopulation *blood_parasite = p->add_new_parasite_to_blood(parasite_type);
    //    std::cout << "hello"<< std::endl;

    double size = model_->random()->random_flat(
            Model::CONFIG->log_parasite_density_level().log_parasite_density_from_liver,
            Model::CONFIG->log_parasite_density_level().log_parasite_density_clinical);

    blood_parasite->set_gametocyte_level(Model::CONFIG->gametocyte_level_full());
    blood_parasite->set_last_update_log10_parasite_density(size);


    double PClinical = p->get_probability_progress_to_clinical();
    double P = model_->random()->random_flat(0.0, 1.0);

    if (P < PClinical) {
        //progress to clinical after several days
        blood_parasite->set_update_function(model_->progress_to_clinical_update_function());
        p->schedule_progress_to_clinical_event_by(blood_parasite);


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

void Population::update() {

    for (PersonIndex *person_index: *person_index_list_) {
        person_index->update();
    }

}

void Population::perform_birth_event() {
    //    std::cout << "Birth Event" << std::endl;

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        double poisson_means = size(loc) * Model::CONFIG->birth_rate() / 360.0;
        int numberOfBirths = Model::RANDOM->random_poisson(poisson_means);

        for (int i = 0; i < numberOfBirths; i++) {
            give_1_birth(loc);
            Model::DATA_COLLECTOR->update_person_days_by_years(loc, (360 - Model::SCHEDULER->current_day_in_year()));
        }
    }

    //    std::cout << "End Birth Event" << std::endl;
}

void Population::give_1_birth(const int &location) {
    auto p = new Person();
    p->init();
    p->set_age(0);
    p->set_host_state(Person::SUSCEPTIBLE);
    p->set_age_class(0);
    p->set_location(location);
    p->set_residence_location(location);
    //    p->residentLocation = location;
    //    p->indexInDeepestNestedVector = vPersonIndex[location][SusceptibleState][0].size();
    //    p->population = this;
    //initial immune value
    p->immune_system()->set_immune_component(new InfantImmuneComponent());
    p->immune_system()->set_latest_immune_value(1.0);
    p->immune_system()->set_increase(false);

    p->set_latest_update_time(Model::SCHEDULER->current_time());
    //                    p->draw_random_immune();

    //set_relative_biting_rate
    p->set_bitting_level(Model::RANDOM->random_biting_level());
    p->set_base_bitting_level_value(
            Model::CONFIG->relative_bitting_information().v_biting_level_value[p->bitting_level()]);
    //    p->update_bitting_level();

    p->set_moving_level(Model::RANDOM->random_moving_level());
    p->set_external_population_moving_level(Model::RANDOM->random_external_population_moving_level());

    p->set_birthday(Model::SCHEDULER->current_time());
    BirthdayEvent::schedule_event(Model::SCHEDULER, p, p->birthday() + 360);

    //schedule for switch
    SwitchImmuneComponentEvent::schedule_for_switch_immune_component_event(Model::SCHEDULER, p,
                                                                           p->birthday() + 360 / 2);

    //    p->startLivingTime = (Global::startTreatmentDay > Global::scheduler->currentTime) ? Global::startTreatmentDay : Global::scheduler->currentTime;
    p->schedule_update_every_K_days_event(Model::CONFIG->update_frequency());

    add_person(p);
}

void Population::perform_death_event() {
    //    std::cout << "Death Event" << std::endl;
    //simply change state to dead and release later
    auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
    if (pi == nullptr) return;

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            if (hs == Person::DEAD) continue;
            for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                int size = pi->vPerson()[loc][hs][ac].size();
                if (size == 0) continue;
                double poisson_means = size * Model::CONFIG->death_rate_by_age()[ac] / 360;

                assert(Model::CONFIG->death_rate_by_age().size() == Model::CONFIG->number_of_age_classes());
                int numberOfDeaths = Model::RANDOM->random_poisson(poisson_means);
                if (numberOfDeaths == 0) continue;

                //                std::cout << numberOfDeaths << std::endl;
                for (int i = 0; i < numberOfDeaths; i++) {
                    //change state to Death;
                    int index = Model::RANDOM->random_uniform(size);
                    //                    std::cout << index << "-" << pi->vPerson()[loc][hs][ac].size() << std::endl;
                    Person *p = pi->vPerson()[loc][hs][ac][index];
                    p->cancel_all_events_except(nullptr);
                    p->set_host_state(Person::DEAD);
                }
            }
        }
    }
    //    std::cout << "Actual delete " << std::endl;
    clear_all_dead_state_individual();
    //    std::cout << "End Actual delete " << std::endl;
}

void Population::clear_all_dead_state_individual() {
    //return all Death to object pool and clear vPersonIndex[l][dead][ac] for all location and ac
    auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
    PersonPtrVector removePersons;

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
            for (auto person : pi->vPerson()[loc][Person::DEAD][ac]) {
                removePersons.push_back(person);
            }
        }
    }

    for (Person *p: removePersons) {
        remove_dead_person(p);
    }
}

void Population::perform_circulation_event() {
    //for each location
    // get number of circulations based on size * circulation_percent
    // distributes that number into others location based of other location size
    // for each number in that list select an individual, and schedule a movement event on next day
    PersonPtrVector today_circulations;

    std::vector<int> v_number_of_residents_by_location(Model::CONFIG->number_of_locations(), 0);

    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
//        v_number_of_residents_by_location[target_location] = (size(target_location));
        v_number_of_residents_by_location[location] = Model::DATA_COLLECTOR->popsize_residence_by_location()[location];
        //        std::cout << v_original_pop_size_by_location[target_location] << std::endl;
    }


    for (int from_location = 0; from_location < Model::CONFIG->number_of_locations(); from_location++) {
        double poisson_means = size(from_location) * Model::CONFIG->circulation_information().circulation_percent;
        if (poisson_means == 0)continue;
        int number_of_circulating_from_this_location = Model::RANDOM->random_poisson(poisson_means);
        if (number_of_circulating_from_this_location == 0) continue;

        DoubleVector v_relative_outmovement_to_destination(Model::CONFIG->number_of_locations(), 0);
        v_relative_outmovement_to_destination = Model::CONFIG->spatial_model()->get_v_relative_outmovement_to_destination(
                from_location, Model::CONFIG->number_of_locations(),
                Model::CONFIG->spatial_distance_matrix()[from_location],
                v_number_of_residents_by_location);

//        for (int target_location = 0; target_location < Model::CONFIG->number_of_locations(); target_location++) {
//            if (target_location == from_location) {
//                v_relative_outmovement_to_destination[target_location] = 0;
//            } else {
//                v_relative_outmovement_to_destination[target_location] = v_original_pop_size_by_location[target_location];
//            }
//        }

        std::vector<unsigned int> v_num_leavers_to_destination(
                static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()));

        Model::RANDOM->random_multinomial(static_cast<int>(v_relative_outmovement_to_destination.size()),
                                          static_cast<unsigned int>(number_of_circulating_from_this_location),
                                          &v_relative_outmovement_to_destination[0], &v_num_leavers_to_destination[0]);

        for (int target_location = 0; target_location < Model::CONFIG->number_of_locations(); target_location++) {
            //            std::cout << v_num_leavers_to_destination[target_location] << std::endl;
            if (v_num_leavers_to_destination[target_location] == 0) continue;
//            std::cout << Model::SCHEDULER->current_time() << "\t" << from_location << "\t" << target_location << "\t"
//                      << v_num_leavers_to_destination[target_location] << std::endl;
            perform_circulation_for_1_location(from_location, target_location,
                                               v_num_leavers_to_destination[target_location],
                                               today_circulations);

        }

    }

    for (Person *p: today_circulations) {
        p->randomly_choose_target_location();
    }

    today_circulations.clear();

}

void Population::perform_circulation_for_1_location(const int &from_location, const int &target_location,
                                                    const int &number_of_circulation,
                                                    PersonPtrVector &today_circulations) {
    DoubleVector vLevelDensity;
    auto pi = get_person_index<PersonIndexByLocationMovingLevel>();

    for (int i = 0; i < Model::CONFIG->circulation_information().number_of_moving_levels; i++) {
        double temp = Model::CONFIG->circulation_information().v_moving_level_value[i] *
                      pi->vPerson()[from_location][i].size();
        vLevelDensity.push_back(temp);
    }

    std::vector<unsigned int> vIntNumberOfCirculation(vLevelDensity.size());

    model_->random()->random_multinomial(static_cast<int>(vLevelDensity.size()),
                                         (unsigned int) number_of_circulation, &vLevelDensity[0],
                                         &vIntNumberOfCirculation[0]);


    for (int moving_level = 0; moving_level < vIntNumberOfCirculation.size(); moving_level++) {
        auto size = static_cast<int>(pi->vPerson()[from_location][moving_level].size());
        if (size == 0) continue;
        for (int j = 0; j < vIntNumberOfCirculation[moving_level]; j++) {


            //select 1 random person from level i
            int index = model_->random()->random_uniform(size);
            Person *p = pi->vPerson()[from_location][moving_level][index];
            assert(p->host_state() != Person::DEAD);

            p->today_target_locations()->push_back(target_location);
            today_circulations.push_back(p);

        }
    }
}

bool Population::has_0_case() {
    auto pi = get_person_index<PersonIndexByLocationStateAgeClass>();
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int hs = Person::EXPOSED; hs <= Person::CLINICAL; hs++) {
            for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                if (!pi->vPerson()[loc][hs][ac].empty()) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Population::initialize_person_indices() {

    int number_of_location = Model::CONFIG->number_of_locations();
    int number_of_hoststate = Person::NUMBER_OF_STATE;
    int number_of_ageclasses = Model::CONFIG->number_of_age_classes();

    auto p_index_by_l_s_a = new PersonIndexByLocationStateAgeClass(number_of_location,
                                                                   number_of_hoststate,
                                                                   number_of_ageclasses);
    person_index_list_->push_back(p_index_by_l_s_a);

    auto p_index_location_bitting_level = new PersonIndexByLocationBittingLevel(
            number_of_location, Model::CONFIG->relative_bitting_information().number_of_biting_levels);
    person_index_list_->push_back(p_index_location_bitting_level);

    auto p_index_location_moving_level = new PersonIndexByLocationMovingLevel(
            number_of_location, Model::CONFIG->circulation_information().number_of_moving_levels);
    person_index_list_->push_back(p_index_location_moving_level);

    //ad external population moving level here   
    auto p_index_location_external_moving_level = new PersonIndexByLocationExternalPopulationMovingLevel(
            number_of_location, Model::CONFIG->external_population_circulation_information().number_of_moving_levels);
    person_index_list_->push_back(p_index_location_external_moving_level);
}

void Population::perform_moving_to_external_population_event() {

    PersonPtrVector today_move_to_external_population;

    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        ///find number of movements to external population

        double poisson_means = size(location) *
                               Model::CONFIG->external_population_circulation_information().circulation_percent[location];
        if (poisson_means == 0)continue;
        int number_of_movements = Model::RANDOM->random_poisson(poisson_means);


        DoubleVector vLevelDensity;
        auto pi = get_person_index<PersonIndexByLocationMovingLevel>();

        for (int i = 0; i < Model::CONFIG->external_population_circulation_information().number_of_moving_levels; i++) {
            double temp = Model::CONFIG->external_population_circulation_information().v_moving_level_value[i] *
                          pi->vPerson()[location][i].size();
            vLevelDensity.push_back(temp);
        }

        std::vector<unsigned int> vIntNumberOfCirculation(vLevelDensity.size());
        model_->random()->random_multinomial(static_cast<const int &>(vLevelDensity.size()),
                                             static_cast<const unsigned int &>(number_of_movements), &vLevelDensity[0],
                                             &vIntNumberOfCirculation[0]);

        for (int moving_level = 0; moving_level < vIntNumberOfCirculation.size(); moving_level++) {
            int size = static_cast<int>(pi->vPerson()[location][moving_level].size());
            if (size == 0) continue;
            for (int j = 0; j < vIntNumberOfCirculation[moving_level]; j++) {


                //select 1 random person from level i
                int index = model_->random()->random_uniform(size);
                Person *p = pi->vPerson()[location][moving_level][index];
                assert(p->host_state() != Person::DEAD);

                //                today_circulations.push_back(p);
                p->set_is_moving_to_external_population(true);
                today_move_to_external_population.push_back(p);
            }
        }

        //schedule for the movement in the next day

        for (Person *p: today_move_to_external_population) {
            if (p->is_moving_to_external_population()) {
                p->set_is_moving_to_external_population(false);
                MoveToExternalPopulationEvent::schedule_event(Model::SCHEDULER, p,
                                                              Model::SCHEDULER->current_time() + 1);
            }

        }

        today_move_to_external_population.clear();
    }
}

void Population::perform_interupted_feeding_recombination() {
    // calculate vector Y, Z
    DoubleVector2 Y = DoubleVector2(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()),
                                    DoubleVector(Model::CONFIG->number_of_parasite_types(), 0));
    DoubleVector2 Z = DoubleVector2(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()),
                                    DoubleVector(Model::CONFIG->number_of_parasite_types(), 0));

    //expected allel frequencies after recombination
    DoubleVector2 eafar = DoubleVector2(static_cast<unsigned long long int>(Model::CONFIG->number_of_locations()),
                                        DoubleVector(Model::CONFIG->number_of_parasite_types(), 0));


    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int parasite_type_id = 0;
             parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
            interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] = current_force_of_infection_by_location_parasite_type_[loc][parasite_type_id];
            Y[loc][parasite_type_id] =
                    interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] *
                    (1 - Model::CONFIG->fraction_mosquitoes_interrupted_feeding());
            Z[loc][parasite_type_id] =
                    interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] *
                    Model::CONFIG->fraction_mosquitoes_interrupted_feeding();
        }
    }

    int numberOfGametocytaemic = 0;

    auto pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        //hs 2: assymptomatic, 3: clinical
        for (int hs = 2; hs <= 3; hs++) {
            for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
                for (auto p : pi->vPerson()[loc][hs][ac]) {
                    if (p->isGametocytaemic()) {
                        numberOfGametocytaemic++;
                    }
                }
            }
        }
    }

    double sumZ = 0;
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int parasite_type_id = 0;
             parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
            sumZ += Z[loc][parasite_type_id];
        }
    }

    double a = Model::CONFIG->fraction_mosquitoes_interrupted_feeding() * numberOfGametocytaemic / sumZ;

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int parasite_type_id = 0;
             parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
            Z[loc][parasite_type_id] = std::lround(a * Z[loc][parasite_type_id]);
        }
    }

    sumZ = 0;
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int parasite_type_id = 0;
             parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
            sumZ += Z[loc][parasite_type_id];
        }
    }
    //    std::cout << sumZ << " -- " << Model::CONFIG->fraction_mosquitoes_interrupted_feeding() * numberOfGametocytaemic;
    // perform free recombination in Z
    double sumEafar = 0;
    if (sumZ > 0.0001) {
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
                if (Z[loc][i] == 0) continue;
                for (int j = 0; j < Model::CONFIG->number_of_parasite_types(); j++) {
                    if (Z[loc][j] == 0) continue;
                    if (i == j) {
                        double weight = Z[loc][i] * Z[loc][i];
                        eafar[loc][i] += weight;
                        //                        sumEafar += weight;
                    } else {
                        double weight = 2 * Z[loc][i] * Z[loc][j];
                        for (int p = 0; p < Model::CONFIG->number_of_parasite_types(); p++) {
                            if (Model::CONFIG->genotype_db()->mating_matrix()[i][j][p] == 0) continue;
                            eafar[loc][p] += weight * Model::CONFIG->genotype_db()->mating_matrix()[i][j][p];
                            //                            sumEafar += weight * Model::CONFIG->parasite_db()->mating_matrix()[i][j][p];
                        }
                    }
                }
            }
        }

        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
                eafar[loc][i] /= (sumZ * sumZ);
                sumEafar += eafar[loc][i];
            }
        }

        //        double s = 0;
        //normalize eafar 
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
                eafar[loc][i] /= sumEafar;
                //                s += eafar[loc][i];
            }
        }
        //weight Z with eafar and divide by a and calculate current_force_of_infection
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            std::vector<unsigned int> newZ = std::vector<unsigned int>(Model::CONFIG->number_of_parasite_types(), 0);
            Model::RANDOM->random_multinomial(static_cast<const int &>(eafar[loc].size()),
                                              static_cast<const unsigned int &>(sumZ), &eafar[loc][0], &newZ[0]);

            for (int parasite_type_id = 0;
                 parasite_type_id < Model::CONFIG->number_of_parasite_types(); parasite_type_id++) {
                Z[loc][parasite_type_id] = newZ[parasite_type_id] / a;
                interupted_feeding_force_of_infection_by_location_parasite_type_[loc][parasite_type_id] =
                        Y[loc][parasite_type_id] + Z[loc][parasite_type_id];
            }
        }
    }
}