/*
 * DbReporter.cpp
 * 
 * Implementation of the DbReporter class.
 */
#include "DbReporter.h"

#include <fmt/format.h>

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "easylogging++.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/Population.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

void DbReporter::initialize(int job_number, std::string path) {
    // Connect to the database
    conn = new pqxx::connection(Model::CONFIG->connection_string());
    LOG(INFO) << "Connected to " << conn->dbname();

    // Ensure that the database is prepared and grab relevent ids before running
    prepare_configuration();
    prepare_replicate();
    
    // Inform the user that we are running
    LOG(INFO) << fmt::format("Running configuration {}, replicate {}.", config_id, replicate);
}

// Make sure the relevent enteries for the configuration are in the database
void DbReporter::prepare_configuration() {
    // Load the text of the configuration
    std::string filename = Model::MODEL->config_filename();
    std::ifstream input(filename);
    std::stringstream stream;
    while (input >> stream.rdbuf());
    
    // Remove the new lines from the string
    std::string yaml = stream.str();
    yaml.erase(std::remove(yaml.begin(), yaml.end(), '\r'), yaml.end());
    yaml.erase(std::remove(yaml.begin(), yaml.end(), '\n'), yaml.end());
    
    // Check to see if this is a known configuration
    pqxx::work db(*conn);
    std::string query = fmt::format(SELECT_CONFIGURATION, db.quote(yaml));
    pqxx::result result = db.exec(query);
    
    // Note the id if provided
    if (result.size() == 1) {
        config_id = result[0][0].as<int>();
        return;
    }

    // Check for a MD5 collision (unlikely given use case)
    if (result.size() > 1) {
        LOG(ERROR) << "MD5 hash collision adding the configuration file!";
        throw new std::runtime_error("MD5 hash collision");
    }

    // The configuration is unknown, so we need to add it to the database
    LOG(INFO) << "Adding unknown configuration to database.";
    query = fmt::format(INSERT_CONFIGURATION, db.quote(yaml), db.quote(yaml));
    result = db.exec(query);
    config_id = result[0][0].as<int>();

    // Prepare the loader query
    query = "";
    for (auto ndx = 0; ndx < Model::CONFIG->number_of_locations(); ndx++) {
        query.append(fmt::format(INSERT_LOCATION, config_id, ndx, Model::CONFIG->location_db()[ndx].beta));
    }

    // Update the database
    db.exec(query);
    db.commit();
}

// Make sure the relevent enteries for this replicate are in the database
void DbReporter::prepare_replicate() {
    // Insert the replicate into the database
    std::string query = fmt::format(INSERT_REPLICATE, config_id, Model::RANDOM->seed());
    pqxx::work db(*conn);
    pqxx::result result = db.exec(query);
    replicate = result[0][0].as<int>();

    // Load the location information
    int locations = Model::CONFIG->number_of_locations();
    location_index = new int[locations];
    pqxx::result results = db.exec(fmt::format(SELECT_LOCATION, config_id));
    for (auto ndx = 0; ndx < locations; ndx++) {
        location_index[ndx] = results[ndx][0].as<int>();
    }

    // Commit prior work and let the user know we are running
    db.commit();
}

// Prepare the monthly report and insert the results in the database
void DbReporter::monthly_report() {
    // Prepare the summary query
    auto days_elapsed = Model::SCHEDULER->current_time();
    auto model_time = std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date);
    auto seasonal_factor = Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0);
    auto treatment_failures = 0;
    auto beta = 0;
    std::string query = fmt::format(INSERT_COMMON, replicate, days_elapsed, model_time, seasonal_factor, treatment_failures, beta);

    // Run the query and grab the id
    pqxx::work db(*conn);
    pqxx::result result = db.exec(query);
    auto id = result[0][0].as<int>();
    
    // Prepare the queries to be run
    query = "";
    monthly_site_data(id, query);
    monthly_genome_data(id, query);

    // Run then and commit everything
    db.exec(query);
    db.commit();
}

// Iterate over all the sites and prepare the query for the site specific genome data
void DbReporter::monthly_genome_data(int id, std::string &query) {
    // Grab the person index
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();

    // Prepare the data structures
    auto genotypes = Model::CONFIG->number_of_parasite_types();
    std::vector<int> individual(genotypes, 0);

    // Iterate over all of the possible locations
    for (auto location = 0; location < index->vPerson().size(); location++) {
        std::vector<int> site(genotypes, 0);
        std::vector<double> frequency(genotypes, 0.0);
        auto sum = 0.0;

        // Iterate over all of the possible states
        for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            // Iterate overall of the age classes
            for (auto ac = 0; ac < index->vPerson()[0][0].size(); ac++) {
                for (auto i = 0ull; i < index->vPerson()[location][hs][ac].size(); i++) {

                    // Get the person, press on if they are not infected (i.e., no parasites)
                    auto* person = index->vPerson()[location][hs][ac][i];
                    if (person->all_clonal_parasite_populations()->parasites()->empty()) {
                        continue;
                    }

                    // Update count of parasites
                    sum += 1;

                    // Count the genotypes present in the individuals
                    for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
                        site[parasite_population->genotype()->genotype_id()]++;
                        individual[parasite_population->genotype()->genotype_id()]++;
                    }

                    // Update the frequency and reset the individual count
                    for (int ndx = 0; ndx < genotypes; ndx++) {
                        frequency[ndx] += (individual[ndx] / static_cast<double>(person->all_clonal_parasite_populations()->parasites()->size()));
                        individual[ndx] = 0;
                    }
                }
            }
        }

        // Prepare and append the query, pass if the genotype was not seen
        for (int genotype = 0; genotype < genotypes; genotype++) {
            if (frequency[genotype] == 0) { 
                continue; 
            }
            query.append(fmt::format(INSERT_GENOTYPE, id, location_index[location], genotype, site[genotype], (frequency[genotype] / sum)));
        }
    }
}

// Iterate over all the sites and prepare the query for the site specific data
void DbReporter::monthly_site_data(int id, std::string &query) {
    for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        // Determine the EIR
        auto eir = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty() 
            ? 0 : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
        eir = (std::isnan(eir)) ? 0 : eir;
        
        // Make sure the PfPR have valid bounds
        auto pfpr_under5 = Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * 100;
        pfpr_under5 = (std::isnan(pfpr_under5)) ? 0 : pfpr_under5;
        auto pfpr_2to10 = Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * 100;
        pfpr_2to10 = (std::isnan(pfpr_2to10)) ? 0 : pfpr_2to10;
        auto pfpr_all = Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * 100;
        pfpr_all = (std::isnan(pfpr_all)) ? 0 : pfpr_all;

        query.append(fmt::format(INSERT_SITE,
            id,
            location_index[location],
            Model::DATA_COLLECTOR->popsize_by_location()[location],
            Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location],
            Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location],
            -1,
            eir,
            pfpr_under5,
            pfpr_2to10,
            pfpr_all
        ));
    }
}

void DbReporter::after_run() {
    // Report that we are done
    pqxx::work db(*conn);
    db.exec(fmt::format(UPDATE_REPLICATE, replicate));
    db.commit();

    // Close the connection 
    conn->close();
}