#include "DbReporter.h"

#include <pqxx/pqxx> 

#include "easylogging++.h"

void DbReporter::initialize(int job_number, std::string path) {
    // Connect to the database
    pqxx::connection* conn = new pqxx::connection("host=localhost dbname=masim user=sim password=sim");
    LOG(INFO) << "Connected to " << conn->dbname();

    // Work with it
    pqxx::work db(*conn);
    db.exec("INSERT INTO test (value) VALUES (1)");
    db.commit();

    // Close the connection 
    conn->close();
}