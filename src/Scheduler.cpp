/* 
 * File:   Scheduler.cpp
 * Author: nguyentran
 * 
 * Created on March 22, 2013, 2:27 PM
 */

#include "Scheduler.h"
#include "Event.h"
#include "HelperFunction.h"
#include "Dispatcher.h"
#include "Model.h"
#include "Config.h"
#include "Population.h"
#include "MDC/ModelDataCollector.h"
#include "Strategies/IStrategy.h"
#include "TMEScheduler.h"
#include "ImportationPeriodicallyEvent.h"
#include <boost/foreach.hpp>

Scheduler::Scheduler(Model *model) :
        model_(model), total_time_(-1), current_time_(-1), is_force_stop_(false) {

}


Scheduler::~Scheduler() {

    //    for(EventPtrVector events_list :  timed_events_list_) {
    //
    //        for(Event* event :  events_list) {
    //            DeletePointer<Event>(event);
    //        }
    //        events_list.clear();
    //    }
    //    timed_events_list_.clear();
    //    
    clear_all_events();
}

void Scheduler::clear_all_events() {

    for (EventPtrVector &events_list :  timed_events_list_) {
        for (Event *event :  events_list) {
            if (event->dispatcher() != nullptr) {
                event->dispatcher()->remove(event);
            }
            DeletePointer<Event>(event);
        }
        events_list.clear();
    }
    timed_events_list_.clear();
}

int Scheduler::total_time() const {
    return total_time_;
}

void Scheduler::set_total_time(const int &value) {
    if (total_time_ > 0) {
        clear_all_events();
    }

    total_time_ = value;
    timed_events_list_.assign(total_time_, EventPtrVector());
}

void Scheduler::schedule(Event *event) {
    // Schedule event in the future
    //1. Compare current time with event time
    //2. Event time cannot exceed total time or less than and equal to current time
    if (event->time() > total_time_ || event->time() <= current_time_) {
        std::cout << "Error to schedule event " << event->name() << " at " << event->time() << "with current_time: "
                  << current_time_ << std::endl;
        DeletePointer<Event>(event);
    } else {
        timed_events_list_[event->time()].push_back(event);
        event->set_scheduler(this);
    }
}

void Scheduler::cancel(Event *event) {
    event->set_executable(false);
}

void Scheduler::run() {
    current_time_ = 0;
    for (current_time_ = 0; !can_stop(); current_time_++) {
        //        std::cout << "Day: " << current_time_ << std::endl;
        begin_time_step();
        if (current_time_ % 30 == 0) {
            perform_monthly_update();
        }

        int size = timed_events_list_[current_time_].size();
        for (int i = 0; i < size; i++) {
            timed_events_list_[current_time_][i]->perform_execute();
            DeletePointer<Event>(timed_events_list_[current_time_][i]);
        }

        timed_events_list_[current_time_].clear();
        //actual release memory
        std::vector<Event *>(timed_events_list_[current_time_]).swap(timed_events_list_[current_time_]);

        end_time_step();
    }
}

void Scheduler::begin_time_step() {
    if (model_ != nullptr) {
        Model::DATA_COLLECTOR->begin_time_step();

        //turn on artemnisinin mutation at intervention day
        if (current_time_ == Model::CONFIG->start_intervention_day()) {
            Model::CONFIG->drug_db()->drug_db()[0]->set_p_mutation(0.0005);
        }

        model_->report_begin_of_time_step();
        model_->perform_infection_event();
    }
}

void Scheduler::end_time_step() {


    Model::POPULATION->perform_birth_event();
    Model::POPULATION->perform_death_event();
    //    Model::EXTERNAL_POPULATION->perform_death_event();
    ///for safety remove all dead by calling perform_death_event
    Model::POPULATION->perform_circulation_event();

    //    Model::POPULATION->perform_moving_to_external_population_event();

    //    Model::TME_SCHEDULER->check_and_perform_TME_Actions();

    update_end_of_time_step();

    report_end_of_time_step();
    Model::DATA_COLLECTOR->perform_yearly_update();
    Model::DATA_COLLECTOR->perform_monthly_update();
}

void Scheduler::update_end_of_time_step() {
    //update / calculate daily UTL
    Model::DATA_COLLECTOR->end_of_time_step();

    //update force of infection
    update_force_of_infection();

    //check to switch strategy
    Model::CONFIG->strategy()->update_end_of_time_step();

    //update treatment coverage
    update_treatment_coverage();


}

void Scheduler::report_end_of_time_step() {
    model_->report_end_of_time_step();
}

bool Scheduler::can_stop() {
    //    std::cout << current_time_ << "\t" << Model::CONFIG->total_time() << std::endl;
    return current_time_ > Model::CONFIG->total_time() || is_force_stop_;
    //        return (current_time_ > Model::CONFIG->total_time()) ||  is_force_stop_;
}

void Scheduler::initialize(const int &total_time) {
    set_total_time(total_time);
    set_current_time(0);
}

void Scheduler::update_force_of_infection() {
    Population *population = model_->population();
    population->perform_interupted_feeding_recombination();

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
        for (int pType = 0; pType < Model::CONFIG->number_of_parasite_types(); pType++) {
            population->force_of_infection_for7days_by_location_parasite_type()[current_time_ %
                                                                                Model::CONFIG->number_of_tracking_days()][loc][pType] = population->interupted_feeding_force_of_infection_by_location_parasite_type()[loc][pType];
        }
    }
}

int Scheduler::current_day_in_year() {
    return (current_time_ - Model::CONFIG->start_collect_data_day()) % 360;
}

void Scheduler::perform_monthly_update() {

}

void Scheduler::update_treatment_coverage() {

    if (current_time_ > Model::CONFIG->start_intervention_day() &&
        (current_time_ - Model::CONFIG->start_intervention_day()) % 360 == 0) {
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            Model::CONFIG->location_db()[loc].p_treatment_less_than_5 *= Model::CONFIG->inflation_factor();
            Model::CONFIG->location_db()[loc].p_treatment_more_than_5 *= Model::CONFIG->inflation_factor();
        }
    }

}
