/* 
 * File:   ProgressToClinicalEvent.cpp
 * Author: Merlin
 * 
 * Created on July 30, 2013, 2:36 PM
 */

#include "ProgressToClinicalEvent.h"
#include "Population/Person.h"
#include "Core/Scheduler.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Population/Population.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"
#include "Therapies/SCTherapy.h"
#include "Population/ClonalParasitePopulation.h"
#include "Core/Random.h"
#include "MDC/ModelDataCollector.h"

OBJECTPOOL_IMPL(ProgressToClinicalEvent)

ProgressToClinicalEvent::ProgressToClinicalEvent() : clinical_caused_parasite_(nullptr) {}

ProgressToClinicalEvent::~ProgressToClinicalEvent() = default;

void ProgressToClinicalEvent::execute() {
  auto *person = dynamic_cast<Person *>(dispatcher);
  if (person->all_clonal_parasite_populations()->size()==0) {
    //parasites might be cleaned by immune system or other things else
    return;
  }

  //if the clinical_caused_parasite eventually removed then do nothing
  if (!person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
    return;
  }

  if (person->host_state()==Person::CLINICAL) {
    clinical_caused_parasite_->set_update_function(Model::MODEL->immunity_clearance_update_function());
    return;
  }


  //    Model* model = person->population()->model();
  //    Random* random = model->random();
  //    Config* config = model->config();

  const auto density = Model::RANDOM->random_uniform_double(
      Model::CONFIG->parasite_density_level().log_parasite_density_clinical_from,
      Model::CONFIG->parasite_density_level().log_parasite_density_clinical_to);

  clinical_caused_parasite_->set_last_update_log10_parasite_density(density);

  // Person change state to Clinical
  person->set_host_state(Person::CLINICAL);

  //this event affect other parasites in population
  //only the parasite that will go to clinical will be change to noneUpdate function,
  //P go to clearance will not be change
  //cancel all other progress to clinical events except current
  person->cancel_all_other_progress_to_clinical_events_except(this);

  person->change_all_parasite_update_function(Model::MODEL->progress_to_clinical_update_function(),
                                              Model::MODEL->immunity_clearance_update_function());
  clinical_caused_parasite_->set_update_function(Model::MODEL->clinical_update_function());

  //Statistic collect cumulative clinical episodes
  Model::DATA_COLLECTOR->collect_1_clinical_episode(person->location(), person->age_class());

  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  const auto p_treatment = Model::TREATMENT_COVERAGE->get_probability_to_be_treated(
      person->location(), person->age());

  // std::cout << p_treatment << std::endl;
  if (p <= p_treatment) {
    auto *therapy = Model::TREATMENT_STRATEGY->get_therapy(person);

    person->receive_therapy(therapy, clinical_caused_parasite_);
    //Statistic increase today treatments
    Model::DATA_COLLECTOR->record_1_treatment(person->location(), therapy->id());

    clinical_caused_parasite_->set_update_function(Model::MODEL->having_drug_update_function());

    // calculate EAMU
    // DEPRECATED CALL
    //Model::DATA_COLLECTOR->record_AMU_AFU(person, therapy, clinical_caused_parasite_);

    // death is 90% lower than no treatment
    if (person->will_progress_to_death_when_recieve_treatment()) {

      //for each test treatment failure event inside individual
      // record treatment failure (not tf)
      //            person->record_treatment_failure_for_test_treatment_failure_events();

      //no treatment routine
      receive_no_treatment_routine(person);

      person->cancel_all_events_except(nullptr);
      person->set_host_state(Person::DEAD);
      Model::DATA_COLLECTOR->record_1_malaria_death(person->location(), person->age_class());
      Model::DATA_COLLECTOR->record_1_TF(person->location(), true);
      Model::DATA_COLLECTOR->record_1_treatment_failure_by_therapy(person->location(), person->age_class(),
                                                                   therapy->id());
      return;
    }

    person->schedule_update_by_drug_event(clinical_caused_parasite_);

    person->schedule_end_clinical_event(clinical_caused_parasite_);
    person->schedule_test_treatment_failure_event(clinical_caused_parasite_, Model::CONFIG->tf_testing_day(),
                                                  therapy->id());

  } else {
    // Did not receieve treatment
    //Statistic store NTF
    Model::DATA_COLLECTOR->record_1_TF(person->location(), false);
    Model::DATA_COLLECTOR->record_1_non_treated_case(person->location(), person->age_class());

    receive_no_treatment_routine(person);
    if (person->host_state()==Person::DEAD) {
      Model::DATA_COLLECTOR->record_1_malaria_death(person->location(), person->age_class());
      return;
    }
    //
    //        //schedule for end of clinical event
    //        std::cout << "progress clinical event" << std::endl;

    person->schedule_end_clinical_by_no_treatment_event(clinical_caused_parasite_);
  }
}

void ProgressToClinicalEvent::schedule_event(Scheduler *scheduler, Person *p,
                                             ClonalParasitePopulation *clinical_caused_parasite, const int &time) {
  if (scheduler!=nullptr) {
    auto *e = new ProgressToClinicalEvent();
    e->dispatcher = p;
    e->set_clinical_caused_parasite(clinical_caused_parasite);
    e->time = time;

    p->add(e);
    scheduler->schedule_individual_event(e);
  }
}

void ProgressToClinicalEvent::receive_no_treatment_routine(Person *p) {
  if (p->will_progress_to_death_when_receive_no_treatment()) {
    p->cancel_all_events_except(nullptr);
    p->set_host_state(Person::DEAD);
  }
}
