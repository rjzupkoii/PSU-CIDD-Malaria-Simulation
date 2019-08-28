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
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/Population.h"

void SpatialReporter::initialize(int job_number, std::string path) {
    // Note the settings
    this->job_number = job_number;
    this->path = path;

    // Open up the monthly report writer, default buffer should be fine
    if (mkdir(path.c_str(), 0777) == 0) {
        LOG(INFO) << "Created output directory, " << path;
    }
    monthly = std::ofstream(fmt::format("{}monthly_data_{}.tsv", path, this->job_number));
    if (!monthly.is_open()) {
        LOG(WARNING) << "Monthly file stream not created!";
    }

    // Prepare the timestamp
    auto end = std::chrono::system_clock::now();
    time = std::chrono::system_clock::to_time_t(end);

    // Write relevent header information
    write_monthly_header();

    // TODO Determine how to switch between TSV and GeoTIFF
    // TODO for now we are assuming TSV

    // Prepare the EIR and PfPR output
    std::string extension = "tsv";
    eir_pfpr = std::ofstream(fmt::format("{}eir_pfpr_{}.{}", path, this->job_number, extension));
    write_eir_pfpr_header(eir_pfpr);
}

void SpatialReporter::monthly_report() {
    
    // Write the basic monthly report
    monthly << Model::SCHEDULER->current_time() << sep;
    monthly << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << sep;
    monthly << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << sep;
    monthly << Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << sep;
    monthly << Model::POPULATION->size() << sep;
    monthly << end_line;

    // TODO Switch between TSV and GeoTIFF as configured

    // Write the EIR and PfPR data
    export_eir_pfpr_table(eir_pfpr);
}

void SpatialReporter::after_run() {
    // Open up the final report writer
    std::ofstream annual = std::ofstream(fmt::format("{}summary_{}.tsv", path, this->job_number));
    
    // Write the header for the file
    annual << std::ctime(&time) << end_line;
    annual << "Random Seed" << sep;
    annual << "Num. Locations" << sep;
    annual << "Beta" << sep;
    annual << "Population Size" << sep;
    annual << "Strategy Id" << sep;
    annual << "Precent Failures" << sep;
    annual << end_line;
    
    // Write the summaries
    annual << Model::RANDOM->seed() << sep;
    annual << Model::CONFIG->number_of_locations() << sep;
    annual << Model::CONFIG->location_db()[0].beta << sep;
    annual << Model::CONFIG->location_db()[0].population_size << sep;
    annual << Model::TREATMENT_STRATEGY->id << sep;
    annual << calculate_treatment_failures() << sep;
    annual << end_line;
    
    // TODO Switch between TSV and GeoTIFF as configured

    // Write the EIR and PfPR data
    export_eir_pfpr_table(eir_pfpr);

    // Close any open writers
    annual.close();
    eir_pfpr.close();
    monthly.close();
}

// Export the EIR and PfPR as a TSV to the stream provided
void SpatialReporter::export_eir_pfpr_table(std::ofstream &out) {
    for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        // Time and location
        out << Model::SCHEDULER->current_time() << sep;
        out << location << sep;

        // Output the EIR
        auto eir = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty() ? 0 :
                   Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
        out << eir << sep;
        
        // PfPR < 5, 2 - 10, and all
        out << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * 100 << sep;
        out << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * 100 << sep;
        out << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * 100 << sep;

        // Infection information
        out << Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[location] << sep;
        out << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location] << sep;
        out << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location] << sep;

        // End line
        out << end_line;
    }
}

// Write the columns that will appear in the EIR / PfPR file
void SpatialReporter::write_eir_pfpr_header(std::ofstream &out) {
    // Note the time stamp
    out << std::ctime(&time) << end_line;

    // Write the header
    out << "Days Elapsed" << sep;
    out << "Location ID" << sep;
    out << "EIR" << sep;
    out << "PfPR < 5" << sep;
    out << "PfPR 2 - 10" << sep;
    out << "PfPR all" << sep;
    out << "New Infections" << sep;
    out << "Treatments" << sep;
    out << "Clinical Episodes" << sep;
    out << end_line;
}

void SpatialReporter::write_monthly_header() {
    // Note the time stamp
    monthly << std::ctime(&time) << end_line;

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
    monthly << end_line;
}