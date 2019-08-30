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
#include "GenotypeReporter.h"
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

    // Prepare the genotype stream, write the time
    genotypes = std::ofstream(fmt::format("{}genotype_data_{}.tsv", path, this->job_number));
    genotypes << std::ctime(&time) << Tsv::end_line;
    GenotypeReporter::output_genotype_weighted_header(genotypes);

    // TODO Determine how to switch between TSV and GeoTIFF
    // TODO for now we are assuming TSV

    // Prepare the EIR and PfPR output
    eir_pfpr = std::ofstream(fmt::format("{}eir_pfpr_{}.{}", path, this->job_number, Tsv::extension));
    write_eir_pfpr_header(eir_pfpr);
}

void SpatialReporter::monthly_report() {
    
    // Write the basic monthly report
    monthly << Model::SCHEDULER->current_time() << Tsv::sep;
    monthly << std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date) << Tsv::sep;
    monthly << date::format("%Y\t%m\t%d", Model::SCHEDULER->calendar_date) << Tsv::sep;
    monthly << Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0) << Tsv::sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 1) << Tsv::sep;
    monthly << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(0, 10) << Tsv::sep;
    monthly << Model::POPULATION->size() << Tsv::sep;
    monthly << Tsv::end_line;

    // Write the genotype information
    GenotypeReporter::output_genotype_weighted(genotypes);

    // TODO Switch between TSV and GeoTIFF as configured

    // Write the EIR and PfPR data
    export_eir_pfpr_table(eir_pfpr);
}

void SpatialReporter::after_run() {
    // Open up the final report writer
    std::ofstream annual = std::ofstream(fmt::format("{}summary_{}.tsv", path, this->job_number));
    
    // Write the header for the file
    annual << std::ctime(&time) << Tsv::end_line;
    annual << "Random Seed" << Tsv::sep;
    annual << "Num. Locations" << Tsv::sep;
    annual << "Beta" << Tsv::sep;
    annual << "Population Size" << Tsv::sep;
    annual << "Strategy Id" << Tsv::sep;
    annual << "Precent Failures" << Tsv::sep;
    annual << Tsv::end_line;
    
    // Write the summaries
    annual << Model::RANDOM->seed() << Tsv::sep;
    annual << Model::CONFIG->number_of_locations() << Tsv::sep;
    annual << Model::CONFIG->location_db()[0].beta << Tsv::sep;
    annual << Model::CONFIG->location_db()[0].population_size << Tsv::sep;
    annual << Model::TREATMENT_STRATEGY->id << Tsv::sep;
    annual << calculate_treatment_failures() << Tsv::sep;
    annual << Tsv::end_line;
    
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
        out << Model::SCHEDULER->current_time() << Tsv::sep;
        out << location << Tsv::sep;

        // Output the EIR
        auto eir = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty() ? 0 :
                   Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
        out << eir << Tsv::sep;
        
        // PfPR < 5, 2 - 10, and all
        out << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * 100 << Tsv::sep;
        out << Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * 100 << Tsv::sep;
        out << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * 100 << Tsv::sep;

        // Infection information
        out << Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[location] << Tsv::sep;
        out << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location] << Tsv::sep;
        out << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location] << Tsv::sep;

        // End line
        out << Tsv::end_line;
    }
}

// Write the columns that will appear in the EIR / PfPR file
void SpatialReporter::write_eir_pfpr_header(std::ofstream &out) {
    // Note the time stamp
    out << std::ctime(&time) << Tsv::end_line;

    // Write the header
    out << "Days Elapsed" << Tsv::sep;
    out << "Location ID" << Tsv::sep;
    out << "EIR" << Tsv::sep;
    out << "PfPR < 5" << Tsv::sep;
    out << "PfPR 2 - 10" << Tsv::sep;
    out << "PfPR all" << Tsv::sep;
    out << "New Infections" << Tsv::sep;
    out << "Treatments" << Tsv::sep;
    out << "Clinical Episodes" << Tsv::sep;
    out << Tsv::end_line;
}

void SpatialReporter::write_monthly_header() {
    // Note the time stamp
    monthly << std::ctime(&time) << Tsv::end_line;

    // Write the header
    monthly << "Days Elapsed" << Tsv::sep;
    monthly << "Model Time" << Tsv::sep;
    monthly << "Model Year" << Tsv::sep;
    monthly << "Model Month" << Tsv::sep;
    monthly << "Model Day" << Tsv::sep;
    monthly << "Seasonal Factor" << Tsv::sep;
    monthly << "Coverage, 0 - 1" << Tsv::sep;
    monthly << "Coverage, 0 - 10" << Tsv::sep;
    monthly << "Population" << Tsv::sep;
    monthly << Tsv::end_line;
}