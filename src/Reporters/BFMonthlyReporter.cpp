//
// Created by Nguyen Tran on 3/5/2018.
//

#include "BFMonthlyReporter.h"
#include "../Model.h"
#include "../Config.h"
#include "../MDC/ModelDataCollector.h"
#include "../MDC/ResistanceTracker.h"
#include "../Random.h"
#include "../Strategies/IStrategy.h"

BFMonthlyReporter::BFMonthlyReporter() {

}

BFMonthlyReporter::~BFMonthlyReporter() {

}

void BFMonthlyReporter::initialize() {

}

void BFMonthlyReporter::before_run() {

}


void BFMonthlyReporter::begin_time_step() {

}

void BFMonthlyReporter::after_time_step() {
    if (Model::SCHEDULER->current_time() % Model::CONFIG->report_frequency() == 0) {
        std::cout << Model::SCHEDULER->current_time() << "\t";
        std::cout << Model::CONFIG->seasonal_factor_for_beta(Model::SCHEDULER->current_time()) << "\t";
        print_PfPR_0_5_by_location();

        std::cout << "-1111\t";

        print_monthly_incidence_by_location();

        std::cout << "-1111" << "\t";

        for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
            std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[i] << "\t";
        }

        std::cout << "-1111" << "\t";

        for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
            std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count()[i] << "\t";
        }

        std::cout << "-1111" << "\t";
        for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
            for (int i = 0; i < Model::CONFIG->number_of_parasite_types(); i++) {
                std::cout << Model::DATA_COLLECTOR->resistance_tracker().parasite_population_count_by_location()[loc][i]
                          << "\t";
            }
        }

        std::cout << std::endl;
    }
}


void BFMonthlyReporter::after_run() {
    std::cout << Model::RANDOM->seed() << "\t" << Model::CONFIG->number_of_locations() << "\t";

    //output strategy information
    std::cout << Model::CONFIG->strategy()->id << "\t";

    //output NTF
    double total_time_in_years = (Model::SCHEDULER->current_time() - Model::CONFIG->start_intervention_day()) / 360.0;

    double sumNTF = 0.0;
    ul popSize = 0;
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        sumNTF += Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location];
        popSize += Model::DATA_COLLECTOR->popsize_by_location()[location];
    }

    std::cout << (sumNTF * 100 / popSize) / total_time_in_years << "\t";

//    std::cout << "-1111\t";
//    for (int age = 0; age < 80; age++) {
//        int sum = 0;
//        for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
//            sum += Model::DATA_COLLECTOR->number_of_treatments_by_location_age_year()[location][age];
//
//        }
//        std::cout << sum << "\t";
//    }


    std::cout << std::endl;
}

void BFMonthlyReporter::print_PfPR_0_5_by_location() {
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
        std::cout << Model::DATA_COLLECTOR->get_blood_slide_prevalence(loc, 0, 5) * 100 << "\t";
    }
}

void BFMonthlyReporter::print_monthly_incidence_by_location() {
    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
        std::cout << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << "\t";
    }

    std::cout << "-1111\t";

    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
        std::cout << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << "\t";
    }

}
