/*
 * DbReporter.cpp
 * 
 * Implementation of the DbReporter class.
 */
#include "DbReporter.h"

#include <fmt/format.h>
#include <thread>

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

// Macro to check if a value is infinite, report if it is, and update the value as needed
#define check_inf(value) if (std::isinf(value)) LOG(WARNING) << "Inf caught " << #value; value = std::isinf(value) ? -9999 : value;

pqxx::connection* DbReporter::get_connection() {
    // Getting a connection is straightforward, so this function is larely intended warp retry functionality
    int RETRY_LIMIT = 10;
    int retry_count = 0;

    while (retry_count < RETRY_LIMIT) {
        try {
            return new pqxx::connection(Model::CONFIG->connection_string());
        } catch (const pqxx::broken_connection &e) {
            retry_count++;
            LOG(WARNING) << "Unable to connect to database, try " << retry_count;

            // Sleep for five seconds before retrying
            std::chrono::milliseconds timespan(5000);
            std::this_thread::sleep_for(timespan);
        }
    }

    LOG(ERROR) << "Unable to connect to database, giving up.";
    exit(-1);
}

void DbReporter::initialize(int job_number, std::string path) {
    // Connect to the database
    pqxx::connection* connection = get_connection();
    LOG(INFO) << "Connected to " << connection->dbname();

    // Ensure that the database is prepared and grab relevent ids before running
    prepare_configuration(connection);
    prepare_replicate(connection);
    
    // Close the connection
    connection->close();
    delete connection;

    // Inform the user that we are running
    LOG(INFO) << fmt::format("Running configuration {}, replicate {}.", config_id, replicate);
}

// Make sure the relevent enteries for the configuration are in the database
void DbReporter::prepare_configuration(pqxx::connection* connection) {
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
    pqxx::work db(*connection);
    std::string query = fmt::format(SELECT_CONFIGURATION, db.quote(yaml), db.quote(filename));
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

    // Update the study id if one was provided
    if (Model::MODEL->study_number() != -1) {
        int study_id = Model::MODEL->study_number();
        LOG(INFO) << "Associating study number " << study_id << " with configuration.";
        query = fmt::format(UPDATE_CONFIGURATION_STUDY, study_id, config_id);
        db.exec(query);
    }

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
void DbReporter::prepare_replicate(pqxx::connection* connection) {
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
    pqxx::work db(*connection);
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

    // Core loop for attempting to store the monthly report, if a broken
    // connection occurs, the delay in getting a new connection will 
    // give us a chance at recovery
    int attempt = 0;
    while (attempt < 5) {
        // Return from the function on success
        if (do_monthly_report()) {
            return;
        }
        attempt++;
    }

    // Something went wrong, make sure it is in the logs and end the simulation
    LOG(ERROR) << "Unable to perform monthly report action after " << attempt << " attempts!";
    LOG(ERROR) << "Model timestep: " << Model::SCHEDULER->current_time();
    std::cerr << "FATAL ERROR: unable to commit monthly report!";
    exit(-1);
}

// Perform the actual monthly report,  
bool DbReporter::do_monthly_report() {
    pqxx::connection* connection;

    try {
        // Get the relevent data
        auto days_elapsed = Model::SCHEDULER->current_time();
        auto model_time = std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date);
        auto seasonal_factor = seasonal_info::get_seasonal_factor(Model::SCHEDULER->calendar_date, 0);

        // Only get the treatment failures when comparison starts
        auto treatment_failures = (Model::SCHEDULER->current_time() > Model::CONFIG->start_of_comparison_period()) ? calculate_treatment_failures() : 0;

        // TODO Determine if we want to capture this or not
        auto beta = 0;

        // Prepare the summary query
        std::string query = fmt::format(INSERT_COMMON, replicate, days_elapsed, model_time, seasonal_factor, treatment_failures, beta);

        // Run the query and grab the id
        connection = get_connection();
        pqxx::work db(*connection);
        pqxx::result result = db.exec(query);
        auto id = result[0][0].as<int>();
        
        // Prepare and run the queries to be run
        query = "";
        monthly_site_data(id, query);
        db.exec(query);
        if (Model::CONFIG->record_genome_db()) {
            query = "";
            monthly_genome_data(id, query);
            db.exec(query);
        }

        // Commit the pending data and close with success
        db.commit();
        connection->close();
        delete connection;
        return true;

    } catch (pqxx::broken_connection &ex) {
        // Connection was broken, clean up and return false
        LOG(WARNING) << "Connection to database broken!";
        if (connection != nullptr) {
            delete connection;
        }
        return false;
    }
}

// Iterate over all the sites and prepare the query for the site specific genome data
//
// WARNING This function updates the monthlysitedata with the total count of infected individuals so it MUST 
//         be invoked after monthly_site_data to ensure the data is inserted correctly.
void DbReporter::monthly_genome_data(int id, std::string &query) {

    // Prepare the data structures
    auto genotypes = Model::CONFIG->number_of_parasite_types();
    std::vector<int> individual(genotypes, 0);

    // Cache some values
    PersonIndexByLocationStateAgeClass* index = Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
    auto age_classes = index->vPerson()[0][0].size();

    // Iterate over all of the possible locations
    for (unsigned int location = 0; location < index->vPerson().size(); location++) {
        std::vector<int> occurrences(genotypes, 0);                 // discrete count of occurrences of the parasite genotype
        std::vector<int> clinicalOccurrences(genotypes, 0);         // discrete count of clinical occurrences of the parasite genotype
        std::vector<int> occurrencesZeroToFive(genotypes, 0);       // discrete count of the occurrences of the parasite genotype in individuals 0 - 5
        std::vector<int> occurrencesTwoToTen(genotypes, 0);         // discrete count of the occurrences of the parasite genotype in individuals 2 - 10
        std::vector<double> weightedOccurrences(genotypes, 0.0);    // weighted occurrences of the genotype
        auto infectedIndividuals = 0.0;                             // discrete count of infected individuals in the location

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

                    // Update count of infected individuals
                    infectedIndividuals += 1;

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

                    // Update the weighted occurrences and reset the individual count
                    auto divisor = static_cast<double>(size);
                    for (unsigned int ndx = 0; ndx < genotypes; ndx++) {
                        weightedOccurrences[ndx] += (individual[ndx] / divisor);
                        individual[ndx] = 0;
                    }
                }
            }
        }

        // Prepare and append the query, pass if the genotype was not seen
        //
        // NOTE since the database was updated to store the weighted occurrences and infected individuals, 
        // the weighted frequency is redundent.
        for (unsigned int genotype = 0; genotype < genotypes; genotype++) {
            if (weightedOccurrences[genotype] == 0) { continue; }
            query.append(fmt::format(INSERT_GENOTYPE, id, location_index[location], genotype, occurrences[genotype], 
                                     clinicalOccurrences[genotype], occurrencesZeroToFive[genotype], occurrencesTwoToTen[genotype], 
                                     (weightedOccurrences[genotype] / infectedIndividuals), weightedOccurrences[genotype]));
        }
        query.append(fmt::format(UPDATE_INFECTED_INDIVIDUALS, infectedIndividuals, id, location_index[location]));
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
        //
        // TODO odds are these can be deleted once we have robust unit testing in place to validate the MDC
        check_nan(eir);
        check_inf(eir);

        check_nan(pfpr_under5);
        check_inf(pfpr_under5);

        check_nan(pfpr_2to10);
        check_inf(pfpr_2to10);
        
        check_nan(pfpr_all);
        check_inf(pfpr_all);

        query.append(fmt::format(INSERT_SITE,
            id,
            location_index[location],
            Model::DATA_COLLECTOR->popsize_by_location()[location],
            Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location],
            Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location],
            Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location],
            eir,
            pfpr_under5,
            pfpr_2to10,
            pfpr_all
        ));
    }
}

void DbReporter::after_run() {
    // Report that we are done
    pqxx::connection* connection = new pqxx::connection(Model::CONFIG->connection_string());
    pqxx::work db(*connection);
    db.exec(fmt::format(UPDATE_REPLICATE, replicate));
    db.commit();

    // Close the connection 
    connection->close();
}