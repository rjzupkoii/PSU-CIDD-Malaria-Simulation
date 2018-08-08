/* 
 * File:   MDAAction.cpp
 * Author: Merlin
 * 
 * Created on August 24, 2013, 3:39 PM
 */

#include <cmath>
#include "MDAAction.h"
#include "Model.h"
#include "Scheduler.h"
#include "Population.h"
#include "PersonIndexByLocationStateAgeClass.h"
#include "Person.h"
#include "Core/Random.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"
#include "Events/ReceiveMDATherapyEvent.h"

MDAAction::MDAAction() {
}

MDAAction::~MDAAction() {
}

void MDAAction::perform_weekly_action(const int& location) {
    std::cout << Model::SCHEDULER->current_time() << "\t MDA Action on location " << location << std::endl;
    // for each individual in the location, schedule a day to receive a TME Treatment
    /// based on coverage and duration of mda


    PersonIndexByLocationStateAgeClass* pi_lsa = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    double percent_MDA_treated_per_day = (Model::CONFIG->tme_info().MDA_coverage[location] / Model::CONFIG->tme_info().MDA_duration[location]);
    for (int state = 0; state < Person::NUMBER_OF_STATE - 1; state++) {
        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
            for (int i = 0; i < pi_lsa->vPerson()[location][state][ac].size(); i++) {
                Person* p = pi_lsa->vPerson()[location][state][ac][i];

                double prob_receive_MDA = Model::RANDOM->random_uniform();
                auto day = static_cast<int>(floor(prob_receive_MDA / percent_MDA_treated_per_day) + 1);
                ReceiveMDATherapyEvent::schedule_event(Model::SCHEDULER, p,
                                                    Model::CONFIG->tme_strategy()->get_therapy(p), Model::SCHEDULER->current_time() + day);
                //                std::cout << prob_receive_MDA << "\t" << day << std::endl;
            }

        }
    }

}