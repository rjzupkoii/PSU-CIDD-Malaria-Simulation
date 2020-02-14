/*
 * MovementReporter.cpp
 * 
 * Implimentation of the MovementReporter class. 
 * 
 * NOTE Since we assume this class will mostly be used for testing purposes, there
 *      is an assumed dependency on DbReporter having already initalized the database.
 * 
 * NOTE This code could be significantly optimized; however, since it is only used 
 *      for the inital model validation it hasn't been tuned that much.
 */
#include "MovementReporter.h"

#include "Core/Config/Config.h"
#include "Core/Random.h"
#include "easylogging++.h"
#include "Model.h"

#define COUNT_LIMIT 100

// Add a reported move to the update
void MovementReporter::add_fine_move(int individual, int source, int destination) {
    // Append the move
    fine_update_query.append(fmt::format(INSERT_FINE_MOVE, replicate, Model::SCHEDULER->current_time(), individual, source, destination));

    // Check to see if we should report
    count++;
    if (count >= COUNT_LIMIT) {
        fine_report();
    }
}

// Add a move between districts
void MovementReporter::add_coarse_move(int individual, int source, int destination) {
    movement_counts[source][destination]++;
}

// Prepare and perform a bulk insert of the coarse movements
void MovementReporter::coarse_report() {
    // If we are at the zero time point, just return since we don't anticpate anything to do
    auto timestep = Model::SCHEDULER->current_time();
    if (timestep == 0) { return; }

    // Open the transaction
    pqxx::work db(*conn);

    // Send the inserts as we read the table
    std::string query;
    for (auto source = 0; source < division_count; source++) {
        for (auto destination = 0; destination < division_count; destination++) {
            if (movement_counts[source][destination] == 0) { continue; }
            query = fmt::format(INSERT_COARSE_MOVE, replicate, timestep, movement_counts[source][destination], source, destination);
            db.exec(query);
            movement_counts[source][destination] = 0;
        }
    }

    if (query.empty()) {
        // Issue a warning if there were no movements since zeroed data is not recorded
        LOG(WARNING) << "No movement between districts recorded.";
        db.abort();
    } else {
        db.commit();
    }
}

// Open a connection to the database and get the replicate based on the random seed value
void MovementReporter::initialize(int job_number, std::string path) {
    // Connect to the database
    conn = new pqxx::connection(Model::CONFIG->connection_string());
    
    // Load the replicate or fail out
    pqxx::work db(*conn);
    std::string query = fmt::format(SELECT_REPLICATE, Model::RANDOM->seed());
    pqxx::result result = db.exec(query);
    replicate = result[0][0].as<int>();
    if (replicate == 0) {
        LOG(ERROR) << "Loaded a value of zero for the replicate, expected one or higher.";
        throw new std::runtime_error("Unable to load replicate from database");
    }

    if (Model::MODEL->district_movement()) {
        // Get the district count, prepare the memory. Note that we are assuming that the count
        // is coming from ArcGis which will use one-based indexing. Therefore we need some extra
        // padding on the area to prevent errors. Since we are only inserting actual movement
        // (i.e., district A to district B) cells with zero are not as big of a concern (baring
        // any memory considerations).
        division_count = SpatialData::get_instance().get_district_count();
    } else {
        // Assume we are tracking cells
        division_count = static_cast<int>(Model::CONFIG->number_of_locations());
    }
     VLOG(1) << "Division Count: " << division_count;
    if (division_count != -1) {
        movement_counts = new int*[division_count + 1];
        for (auto ndx = 0; ndx < division_count + 1; ndx++) {
            movement_counts[ndx] = new int[division_count + 1];
            for (auto ndy = 0; ndy < division_count + 1; ndy++) {
                movement_counts[ndx][ndy] = 0;
            }
        }
    }

    // Inform the user that we are running
    LOG(INFO) << fmt::format("MovementReporter loaded, using replicate {}", replicate);
}

// Call the relevent sub reports
void MovementReporter::monthly_report() {
    if (movement_counts != nullptr) { coarse_report(); }
    if (count > 0) { fine_report(); }
}

// Insert the buffered moves into the database
void MovementReporter::fine_report() {
    // Insert the fine movement data
    pqxx::work db(*conn);
    db.exec(fine_update_query);
    db.commit();

    // Reset the query and counter
    fine_update_query = "";
    count = 0;
}

// Close the connection
void MovementReporter::after_run() {
    conn->close();
}