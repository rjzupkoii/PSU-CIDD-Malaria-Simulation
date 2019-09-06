#include "DbReporter.h"

#include <fmt/format.h>

#include "easylogging++.h"
#include "Model.h"

void DbReporter::initialize(int job_number, std::string path) {
    // TODO Get this from the configuration file / command line?

    // Connect to the database
    conn = new pqxx::connection("host=localhost dbname=masim user=sim password=sim");
    LOG(INFO) << "Connected to " << conn->dbname();

    // TODO Check the database to make sure it is what we want
}

void DbReporter::monthly_report() {
    const std::string INSERT = 
    "INSERT INTO sim.MonthlyData "
    "(RunId, DaysElapsed, ModelTime, SeasonalFactor, TreatmentFailures, Beta) "
    "VALUES ({}, {}, {}, {}, {}, {});";

    // Prepare the query
    auto run_id = 0;
    auto days_elapsed = Model::SCHEDULER->current_time();
    auto model_time = std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date);
    auto seasonal_factor = Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0);
    auto treatment_failures = 0;
    auto beta = 0;
    query += fmt::format(INSERT, run_id, days_elapsed, model_time, seasonal_factor, treatment_failures, beta);
    count++;

    // Run the query if it's been a year
    if (count % 12 == 0) {
        pqxx::work db(*conn);
        db.exec(query);
        db.commit();
        query = "";
        count = 0;
    }
}

void DbReporter::after_run() {
    // Run any final query
    pqxx::work db(*conn);
    db.exec(query);
    db.commit();

    // Close the connection 
    conn->close();
}