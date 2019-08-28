/*
 * SpatialReporter.cpp
 * 
 * Implementation of the SpatialReporter class.
 */
#include "SpatialReporter.h"

#include <chrono>
#include <fmt/format.h>
#include <iostream>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "Model.h"
#include "Population/Population.h"

void SpatialReporter::initialize(int job_number, std::string path) {
    // Note the settings
    this->jobNumber = job_number;
    this->path = path;

    // Open up the monthly report writer, default buffer should be fine
    if (mkdir(path.c_str(), 0777) == 0) {
        LOG(INFO) << "Created output directory, " << path;
    }
    this->monthly = std::ofstream(fmt::format("{}monthly_data_{}.tsv", path, jobNumber));
    if (!this->monthly.is_open()) {
        LOG(WARNING) << "Monthly file stream not created!";
    }

    // Write relevent header information
    writeMonthlyHeader();
}

void SpatialReporter::monthly_report() {
    monthly << Model::SCHEDULER->current_time() << sep;
    monthly << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << sep;
    monthly << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << sep;
    monthly << Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << sep;
    monthly << Model::POPULATION->size() << sep;
    monthly << "\n";
}

void SpatialReporter::after_run() {
    // Open up the final report writer
    std::ofstream annual = std::ofstream(fmt::format("{}summary_{}.tsv", path, jobNumber));
    
    // Write the header for the file
    annual << std::ctime(&this->time) << "\n";
    annual << "Random Seed" << sep;
    annual << "Num. Locations" << sep;
    annual << "Beta" << sep;
    annual << "Population Size" << sep;
    annual << "Strategy Id" << sep;
    annual << "Precent Failures" << sep;
    annual << "\n";
    
    // Write the summaries
    annual << Model::RANDOM->seed() << sep;
    annual << Model::CONFIG->number_of_locations() << sep;
    annual << Model::CONFIG->location_db()[0].beta << sep;
    annual << Model::CONFIG->location_db()[0].population_size << sep;
    annual << Model::TREATMENT_STRATEGY->id << sep;
    annual << calculate_treatment_failures() << sep;
    annual << "\n";
    
    // Close any open writers
    annual.close();
    monthly.close();
}

void SpatialReporter::export_eir_pfpr(std::ofstream out) {

    // TODO Write this method!
    
}

void SpatialReporter::write_monthly_header() {
    // Note the time stamp
    auto end = std::chrono::system_clock::now();
    this->time = std::chrono::system_clock::to_time_t(end);
    monthly << std::ctime(&this->time) << "\n";

    // Write the header
    monthly << "Days Elapsed" << sep;
    monthly << "Model Time" << sep;
    monthly << "Model Year" << sep;
    monthly << "Model Month" << sep;
    monthly << "Model Day" << sep;
    monthly << "Seasonal Factor" << sep;
    monthly << "Coverage, 0 - 1" << sep;
    monthly << "Coverage, 0 - 10" << sep;
    monthly << "Population" << sep;
    monthly << "\n";
}