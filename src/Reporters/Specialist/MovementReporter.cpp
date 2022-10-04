/*
 * MovementReporter.cpp
 * 
 * Implementation of the MovementReporter class.
 * 
 * NOTE Since we assume this class will mostly be used for testing purposes, there
 *      is an assumed dependency on DbReporter having already initialized the database.
 * 
 * NOTE This code could be significantly optimized; however, since it is only used 
 *      for the initial model validation it hasn't been tuned that much.
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

// Prepare and perform a bulk insert of the aggregated movements (can be cell-to-cell or district-to-district)
//
// NOTE This is slowed down a lot by sending the query to the DB for each source-destination. For district movements
//      this isn't too bad, but for cellular it can be painfully slow.
void MovementReporter::coarse_report() {
    // If we are at the zero time point, just return since we don't anticipate anything to do
    auto timestep = Model::SCHEDULER->current_time();
    if (timestep == 0) { return; }

    // Open the transaction
    pqxx::work db(*conn);

    // Send the inserts as we read the table, note that we assume that the division count is correct prepared 
    // with padding as needed if it is coming from one-index ArcGIS
    std::string query;
    for (auto source = 0; source < division_count; source++) {
        for (auto destination = 0; destination < division_count; destination++) {
            if (movement_counts[source][destination] == 0) { continue; }
            query = fmt::format(INSERT_COARSE_MOVE, replicate, timestep, movement_counts[source][destination], source, destination);
            movement_counts[source][destination] = 0;
            db.exec(query);
        }
    }

    if (query.empty()) {
        // Issue a warning if there were no movements since zeroed data is not recorded
        LOG(WARNING) << "No movement between districts recorded.";
        db.abort();
        return;
    } 

    // Commit the data
    db.commit();
}

// Open a connection to the database
void MovementReporter::initialize(int job_number, std::string path) {
    // Connect to the database
    conn = new pqxx::connection(Model::CONFIG->connection_string());
    
    // Since we are dependent upon DbReporter, the replicate should already be set
    replicate = Model::MODEL->replicate();
    if (replicate == 0) {
        LOG(ERROR) << "Replicate value of zero, expected one or higher. Was DbReporter loaded?";
        throw std::runtime_error("Invalid replicate value");
    }

    if (Model::MODEL->district_movement()) {
        // Get the district count, prepare the memory. Note that we are assuming that the count
        // is coming from ArcGis which will use one-based indexing. Therefore, we need some extra
        // padding on the area to prevent errors. Since we are only inserting actual movement
        // (i.e., district A to district B) cells with zero are not as big of a concern (baring
        // any memory considerations).
        division_count = SpatialData::get_instance().get_district_count();
        if (division_count == -1) {
            VLOG(1) << "District not loaded";
            return;
        }
        
        // Pad the count
        division_count++;

        VLOG(1) << "Real District Count: " << division_count - 1;
        VLOG(1) << "Adjusted District Count: " << division_count;
    } else {
        // Assume we are tracking cells
        division_count = static_cast<int>(Model::CONFIG->number_of_locations());
        VLOG(1) << "Cell count: " << division_count;
    }
    
    // Prepare the memory, assume that padding is in place
    movement_counts = new int*[division_count];
    for (auto ndx = 0; ndx < division_count; ndx++) {
        movement_counts[ndx] = new int[division_count];
        for (auto ndy = 0; ndy < division_count; ndy++) {
            movement_counts[ndx][ndy] = 0;
        }
    }

    // Inform the user that we are running
    LOG(INFO) << fmt::format("MovementReporter loaded, using replicate {}", replicate);
}

// Call the relevant sub reports
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