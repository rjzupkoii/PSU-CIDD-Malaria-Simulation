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

#define COUNT_LIMIT

// Add a reported move to the update
void MovementReporter::add_move(int individual, int source, int destination) {
    // Append the move
    update_query.append(fmt::format(INSERT_MOVE, replicate, Model::SCHEDULER->current_time(), individual, source, destination));

    // Check to see if we should report
    count++;
    if (count >= COUNT_LIMIT) {
        report();
        count = 0;
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

    // Inform the user that we are running
    LOG(INFO) << fmt::format("MovementReporter loaded, using replicate {}", replicate);
}

// Insert the buffered moves into the database
void MovementReporter::report() {
    // Insert the data
    pqxx::work db(*conn);
    db.exec(update_query);
    db.commit();

    // Reset the query
    update_query = "";
}

// Close the connection
void MovementReporter::after_run() {
    conn->close();
}