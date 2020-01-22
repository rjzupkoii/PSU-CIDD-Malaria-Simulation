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
#include "GIS/SpatialData.h"

// Macro to check to see if a value is NAN, report if it is, and update the value as needed
#define check_nan(value) if (std::isnan(value)) LOG(WARNING) << "NaN caught: " << #value; value = std::isnan(value) ? 0 : value;

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

    // Prepare the correct query depending on if there is a raster or not
    if (SpatialData::get_instance().has_raster()) {
        SpatialData::RasterInformation header = SpatialData::get_instance().get_raster_header();
        query = fmt::format(INSERT_CONFIGURATION_RASTER, db.quote(yaml), db.quote(yaml), db.quote(filename), 
            header.number_columns, header.number_rows, db.quote(header.x_lower_left_corner), db.quote(header.y_lower_left_corner), header.cellsize);        
    } else {
        query = fmt::format(INSERT_CONFIGURATION, db.quote(yaml), db.quote(yaml), db.quote(filename));
    }
    
    // The configuration is unknown, so we need to add it to the database
    LOG(INFO) << "Adding unknown configuration to database.";
    result = db.exec(query);
    config_id = result[0][0].as<int>();

    // Check to see if districts were loaded
    auto districts = SpatialData::get_instance().has_raster(SpatialData::SpatialFileType::Districts);

    // Prepare the loader query
    query = "";
    for (unsigned int ndx = 0; ndx < Model::CONFIG->number_of_locations(); ndx++) {
        auto location = Model::CONFIG->location_db()[ndx];
        auto x = (int)location.coordinate.get()->latitude;
        auto y = (int)location.coordinate.get()->longitude;

        if (districts) {
            auto district = SpatialData::get_instance().get_district(ndx);
            query.append(fmt::format(INSERT_LOCATION_DISTRICT, config_id, ndx, x, y, location.beta, district));
        } else {
            query.append(fmt::format(INSERT_LOCATION, config_id, ndx, x, y, location.beta));
        }
    }

    // Update the database
    db.exec(query);
    db.commit();
}

// Make sure the relevent enteries for this replicate are in the database
void DbReporter::prepare_replicate() {
    // Check to see what type of movement is being recoreded, if any
    char movement = 'X';
    if (Model::MODEL->report_movement()) {
        if (Model::MODEL->individual_movement()) {
            movement = 'I';
        } else if (Model::MODEL->cell_movement()) {
            movement = 'C';
        } else if (Model::MODEL->district_movement()) {
            movement = 'D';
        }
    }

    // Insert the replicate into the database
    std::string query = fmt::format(INSERT_REPLICATE, config_id, Model::RANDOM->seed(), movement);
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

    // Prepare the data structures
    auto genotypes = Model::CONFIG->number_of_parasite_types();
    std::vector<int> individual(genotypes, 0);

    // Cache some values
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    auto age_classes = index->vPerson()[0][0].size();

    // Iterate over all of the possible locations
    for (unsigned int location = 0; location < index->vPerson().size(); location++) {
        std::vector<int> occurrences(genotypes, 0);
        std::vector<int> clinicalOccurrences(genotypes, 0);
        std::vector<int> occurrencesZeroToFive(genotypes, 0);
        std::vector<int> occurrencesTwoToTen(genotypes, 0);
        std::vector<double> frequency(genotypes, 0.0);
        auto sum = 0.0;

        // Iterate over all of the possible states
        for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
            // Iterate over all of the age classes
            for (unsigned int ac = 0; ac < age_classes; ac++) {
                // Iterate over all of the genotypes
                auto age_class = index->vPerson()[location][hs][ac];
                for (auto i = 0ull; i < age_class.size(); i++) {

                    // Get the person, press on if they are not infected (i.e., no parasites)
                    auto parasites = age_class[i]->all_clonal_parasite_populations()->parasites();
                    auto size = parasites->size();
                    if (size == 0) { continue; }

                    // Note the age and clinical status of the person
                    auto age = age_class[i]->age();
                    auto clinical = (int)(age_class[i]->host_state() == Person::HostStates::CLINICAL);

                    // Update count of parasites
                    sum += 1;

                    // Count the genotypes present in the individuals
                    for (unsigned int ndx = 0; ndx < size; ndx++) {
                        auto parasite_population = (*parasites)[ndx];
                        auto id = parasite_population->genotype()->genotype_id();
                        occurrences[id]++;
                        occurrencesZeroToFive[id] += (age <= 5);
                        occurrencesTwoToTen[id] += (age >= 2 && age <= 10);
                        individual[id]++;
                        
                        // TODO Double check how we are counting these
                        clinicalOccurrences[id] += clinical;
                    }

                    // Update the frequency and reset the individual count
                    auto divisor = static_cast<double>(size);
                    for (unsigned int ndx = 0; ndx < genotypes; ndx++) {
                        frequency[ndx] += (individual[ndx] / divisor);
                        individual[ndx] = 0;
                    }
                }
            }
        }

        // Prepare and append the query, pass if the genotype was not seen
        for (unsigned int genotype = 0; genotype < genotypes; genotype++) {
            if (frequency[genotype] == 0) { continue; }
            query.append(fmt::format(INSERT_GENOTYPE, id, location_index[location], genotype, occurrences[genotype], 
                                     clinicalOccurrences[genotype], occurrencesZeroToFive[genotype], occurrencesTwoToTen[genotype], (frequency[genotype] / sum)));
        }
    }
}

// Iterate over all the sites and prepare the query for the site specific data
void DbReporter::monthly_site_data(int id, std::string &query) {
    for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        // Check the population, if there is nobody there, press on
        if (Model::DATA_COLLECTOR->popsize_by_location()[location] == 0) {
            continue;
        }

        // Determine the EIR and PfPR values
        auto eir = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty() 
            ? 0 : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
        auto pfpr_under5 = Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * 100.0;
        auto pfpr_2to10 = Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * 100.0;
        auto pfpr_all = Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * 100.0;
       
        // Make make sure we have valid bounds
        check_nan(eir);
        check_nan(pfpr_under5);
        check_nan(pfpr_2to10);
        check_nan(pfpr_all);

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