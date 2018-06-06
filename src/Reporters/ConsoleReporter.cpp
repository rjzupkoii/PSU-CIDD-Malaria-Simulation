/* 
 * File:   ConsoleReporter.cpp
 * Author: Merlin
 * 
 * Created on August 1, 2013, 12:15 PM
 */

#include "ConsoleReporter.h"
#include "../Model.h"
#include "../Random.h"
#include "../Config.h"
#include "../Population.h"
#include "../PersonIndexByLocationStateAgeClass.h"
#include "../MDC/ModelDataCollector.h"
#include "../Strategies/IStrategy.h"
#include "../Therapy.h"
#include <boost/format.hpp>
#include <iomanip>    

#define COLUMN_WIDTH 5
#define COUT std::cout << std::setw(COLUMN_WIDTH) << std::setprecision(3)

ConsoleReporter::ConsoleReporter() {
}

ConsoleReporter::~ConsoleReporter() {
}

void ConsoleReporter::initialize() {
}

void ConsoleReporter::before_run() {
    std::cout << "Seed:" << Model::RANDOM->seed() << std::endl;

}

void report_number_by_state(const int& location, PersonIndexByLocationStateAgeClass* pi) {


    //    std::cout << std::setw(10) << std::setprecision(3);
    for (int hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
        //        int sum = 0;
        //        for (int ac = 0; ac < Model::CONFIG->number_of_age_classes(); ac++) {
        //            sum += pi->vPerson()[location][hs][ac].size();
        //        }
        double v = Model::DATA_COLLECTOR->popsize_by_location_hoststate()[location][hs] * 100 / (double) Model::DATA_COLLECTOR->popsize_by_location()[location];
        //        double v = sum;
        std::cout << boost::format("%1%") % boost::io::group(std::setw(COLUMN_WIDTH), std::setprecision(3), v) << "\t";
    }





}

void ConsoleReporter::after_run() {
    std::cout << "==========================================================================" << std::endl;

    //total time
    double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_collect_data_day()) / 360.0;
    std::cout << "Total time (from equilibrium) : " << total_time_in_years << " years" << std::endl;

    //report EIR
    std::cout << "EIR by location:" << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        std::cout << Model::DATA_COLLECTOR->EIR_by_location()[location] << "\t";
    }
    std::cout << std::endl;

    //total number of bites
    std::cout << "Number of infectious bites:" << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        std::cout << Model::DATA_COLLECTOR->total_number_of_bites_by_location()[location] << "\t";
    }
    std::cout << std::endl;

    std::cout << "Number of clinical episodes:" << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        std::cout << Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[location] << "\t";
    }
    std::cout << std::endl;

    std::cout << "Percentage of bites on top 20% bitten" << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        std::cout << Model::DATA_COLLECTOR->percentage_bites_on_top_20_by_location()[location]*100 << "%" << "\t";
    }
    std::cout << std::endl;

    std::cout << "NTF by location: " << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        double location_NTF = Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location] * 100 / (double) Model::DATA_COLLECTOR->popsize_by_location()[location];
        location_NTF /= total_time_in_years;

        COUT << location_NTF << "\t";
    }
    std::cout << std::endl;

    std::cout << "Number of mutations by location: " << std::endl;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        COUT << Model::DATA_COLLECTOR->cumulative_mutants_by_location()[location] << "\t";
    }
    std::cout << std::endl;

    for (int t_id = 0; t_id < Model::CONFIG->therapy_db().size(); t_id++) {

        int nTreaments = Model::DATA_COLLECTOR->number_of_treatments_with_therapy_ID()[t_id];
        int nSuccess = Model::DATA_COLLECTOR->number_of_treatments_success_with_therapy_ID()[t_id];
        int nFail = Model::DATA_COLLECTOR->number_of_treatments_fail_with_therapy_ID()[t_id];
        double pSuccess = (nTreaments == 0) ? 0 : nSuccess * 100.0 / nTreaments;

        std::cout << "Number of patients (with non-resistant parasite) treated with therapy " << t_id << " (% success) = " << nTreaments << " (" << pSuccess << "%)" << std::endl;
        std::cout << "Number Failed: " << nFail << "-" << nSuccess << "-" << nTreaments << std::endl;

    }

    std::cout << "Strategy UTL: " << Model::DATA_COLLECTOR->current_utl_duration() << std::endl;

    std::cout << "AMU per parasite population: " << Model::DATA_COLLECTOR->AMU_per_parasite_pop() << std::endl;
    std::cout << "AMU per per: " << Model::DATA_COLLECTOR->AMU_per_person() << std::endl;
    std::cout << "EAMU count only clinical caused parasite: " << Model::DATA_COLLECTOR->AMU_for_clinical_caused_parasite() << std::endl;


    for (int i = 0; i < Model::DATA_COLLECTOR->resistance_tracker().tracking_values().size(); i++) {
        std::cout << "Time until any double resistance reaches " << Model::DATA_COLLECTOR->resistance_tracker().tracking_values()[i]*100 << "%: " << Model::DATA_COLLECTOR->resistance_tracker().any_double_tracking_time()[i] << std::endl;
        std::cout << "Time until all double resistance reaches " << Model::DATA_COLLECTOR->resistance_tracker().tracking_values()[i]*100 << "%: " << Model::DATA_COLLECTOR->resistance_tracker().all_double_tracking_time()[i] << std::endl;
        std::cout << "Time until any triple resistance reaches " << Model::DATA_COLLECTOR->resistance_tracker().tracking_values()[i]*100 << "%: " << Model::DATA_COLLECTOR->resistance_tracker().any_triple_tracking_time()[i] << std::endl;
        std::cout << "Time until all triple resistance reaches " << Model::DATA_COLLECTOR->resistance_tracker().tracking_values()[i]*100 << "%: " << Model::DATA_COLLECTOR->resistance_tracker().all_triple_tracking_time()[i] << std::endl;
        std::cout << "Time until total resistance reaches " << Model::DATA_COLLECTOR->resistance_tracker().tracking_values()[i]*100 << "%: " << Model::DATA_COLLECTOR->resistance_tracker().total_tracking_time()[i] << std::endl;

    }



}

void ConsoleReporter::begin_time_step() {
}

void ConsoleReporter::after_time_step() {

    if (Model::SCHEDULER->current_time() % Model::CONFIG->report_frequency() == 0) {
//        Model::DATA_COLLECTOR->perform_population_statistic();

        std::cout << std::setw(COLUMN_WIDTH) << Model::SCHEDULER->current_time() << "\t";

        PersonIndexByLocationStateAgeClass* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

        for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
            std::cout << std::setw(COLUMN_WIDTH) << "||\t";
            report_number_by_state(location, pi);
            COUT << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]*100 << "\t";
            COUT << Model::DATA_COLLECTOR->total_immune_by_location()[location] / Model::POPULATION->size(location) << "\t";
            COUT << Model::DATA_COLLECTOR->current_RITF_by_location()[location] << "-" << Model::DATA_COLLECTOR->current_TF_by_location()[location] << "\t";
        }
        std::cout << std::endl;
    }
}