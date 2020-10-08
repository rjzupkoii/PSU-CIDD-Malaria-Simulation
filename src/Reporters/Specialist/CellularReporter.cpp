/*
 * CellularReporter.cpp
 * 
 * Implementation of the CellularReporter class.
 */
#include "CellularReporter.h"

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "easylogging++.h"

void CellularReporter::initialize(int job_number, std::string path) {
    // Check the number of cells in the model, if it exceeds one, throw an error
    if (Model::CONFIG->location_db().size() > 1) {
        throw std::runtime_error("CellularReporter can only be used with one location configurations.");
    }

    // Prepare the loggers to record the data
    el::Configurations aggregate_reporter;
    aggregate_reporter.setToDefault();
    aggregate_reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
    aggregate_reporter.setGlobally(el::ConfigurationType::ToFile, "true");
    aggregate_reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}_aggregate_data_{}.csv", path, job_number));
    aggregate_reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    aggregate_reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
    el::Loggers::reconfigureLogger("aggregate_reporter", aggregate_reporter);

    el::Configurations detail_reporter;
    detail_reporter.setToDefault();
    detail_reporter.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
    detail_reporter.setGlobally(el::ConfigurationType::ToFile, "true");
    detail_reporter.setGlobally(el::ConfigurationType::Filename, fmt::format("{}_detailed_data_{}.csv", path, job_number));
    detail_reporter.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    detail_reporter.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
    el::Loggers::reconfigureLogger("detail_reporter", detail_reporter);

    // Log the aggregate headers
    ss << "DaysElapsed" << Csv::sep << "Population" << Csv::sep << "InfectedIndividuals" << Csv::sep 
       << "ParasiteClones" << Csv::sep << "580yWeighted" << Csv::sep << "508yUnweighted" << Csv::end_line;
    CLOG(INFO, "aggregate_reporter") << ss.str();
    ss.str("");

    // Log the detailed headers
    ss << "DaysElapsed" << Csv::sep << "Individual" << Csv::sep;
    for (std::size_t id = 0; id < Model::CONFIG->genotype_db()->size(); id++) {
        auto genotype = (*Model::CONFIG->genotype_db())[id];
        ss << genotype->to_string() << Csv::sep;
    }
    ss << Csv::end_line;
    CLOG(INFO, "detail_reporter") << ss.str();
    ss.str("");
}

void CellularReporter::monthly_report() {
    int _580yCount = 0;
    int infectedIndividuals = 0;
    int parasiteClones = 0;
    int population = 0;
    float _580yWeighted = 0;
    
    // Hold onto the age class index
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

    // Iterate over the population
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
        for (std::size_t ac = 0; ac < index->vPerson()[0][0].size(); ac++) {
            // Iterate over all of the genotypes, there should only be one location (i.e., index 0)
            auto age_class = index->vPerson()[0][hs][ac];
            for (std::size_t ndx = 0; ndx < age_class.size(); ndx++) {
                
                // Get the person and count them
                auto parasites = age_class[ndx]->all_clonal_parasite_populations()->parasites();
                population++;

                // Press on if they are not infected
                auto size = parasites->size();
                if (size == 0) { continue; }

                // Update the infected count
                infectedIndividuals++;

                // Examine genotypes of the parasites present and tabulate when 580Y is expressed
                int count = 0;
                for (std::size_t infection = 0; infection < size; infection++) {
                    auto parasite_population = (*parasites)[infection];

                    // Update the total clones count
                    parasiteClones++;

                    // WARNING - This is hard-coded on the assumption that the K13 Propeller locus is at index 2
                    if (parasite_population->genotype()->gene_expression()[2] == 1) {
                        _580yCount++;
                        count++;
                    }
                }

                // Update the weighted occurrences
                _580yWeighted += (count / static_cast<double>(size));
            }
        }
    }

    // Store the data
    auto dayselapsed = Model::SCHEDULER->current_time();
    ss << Model::SCHEDULER->current_time() << Csv::sep 
       << population << Csv::sep 
       << infectedIndividuals << Csv::sep 
       << parasiteClones << Csv::sep
       << _580yWeighted << Csv::sep 
       << _580yCount << Csv::end_line;
    CLOG(INFO, "aggregate_reporter") << ss.str();
    ss.str("");

    // If we are in the detailed reporting phase, run that function
    //
    // WARNING - this is hard coded for now, assuming the model starts in 2007-01-01 mutations should be enabled 
    //           starting in 2018-01-10, so we want ten years of mutations (i.e., 2028-01-01) for capturing detailed data
    if (dayselapsed > (22 * 365 + 4) && dayselapsed < (23 * 365 + 5)) {
        detailed_report();
    }
}

// Generate a detailed report by recording the infection status of all individuals 
// in the model. The expectation is that this function will be called by monthly_report
// during the appropriate time frame.
void CellularReporter::detailed_report() {

    // Hold on to some values
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    auto dayselapsed = Model::SCHEDULER->current_time();
    auto genotypes = Model::CONFIG->genotype_db()->size();

    // Iterate over the population
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
        for (std::size_t ac = 0; ac < index->vPerson()[0][0].size(); ac++) {
            // Iterate over all of the genotypes, there should only be one location (i.e., index 0)
            auto age_class = index->vPerson()[0][hs][ac];
            for (std::size_t ndx = 0; ndx < age_class.size(); ndx++) {

                // Add the lead to the buffer
                ss << dayselapsed << Csv::sep << &age_class[ndx] << Csv::sep;
                
                // Get the person and count them, if they aren't infected zero out the row
                auto parasites = age_class[ndx]->all_clonal_parasite_populations()->parasites();
                if (parasites->size() == 0) {
                    for (int count = 0; count < genotypes; count++) {
                        ss << 0 << Csv::sep;
                    }
                    ss << Csv::end_line;
                    continue;
                }

                // Get the genotypes for the parasites present in the individual
                std::list<int> genotype_ids;
                for (std::size_t infection = 0; infection < parasites->size(); infection++) {
                    genotype_ids.push_back((*parasites)[infection]->genotype()->genotype_id());
                }

                // Sort the list and update the indicators for this individual, note that there should 
                // only be one occurrance of a specific genotype in the individual due to clones
                genotype_ids.sort();
                for (int count = 0; count < genotypes; count++) {
                    if (genotype_ids.front() == count) {
                        ss << 1 << Csv::sep;
                        genotype_ids.pop_front();
                    } else {
                        ss << 0 << Csv::sep;
                    }
                }
                ss << Csv::end_line;
            }
        }
    }

    // Store the data
    CLOG(INFO, "detail_reporter") << ss.str();
    ss.str("");
}
