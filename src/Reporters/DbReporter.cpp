#include "DbReporter.h"

#include <fmt/format.h>

#include "Core/Config/Config.h"
#include "easylogging++.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"

void DbReporter::initialize(int job_number, std::string path) {
    // TODO Get this from the configuration file / command line?

    // Connect to the database
    conn = new pqxx::connection("host=localhost dbname=masim user=sim password=sim");
    LOG(INFO) << "Connected to " << conn->dbname();

    // TODO Check the database to make sure it is what we want
}

void DbReporter::monthly_report() {
    const std::string INSERT_COMMON = 
    "INSERT INTO sim.MonthlyData "
    "(RunId, DaysElapsed, ModelTime, SeasonalFactor, TreatmentFailures, Beta) "
    "VALUES ({}, {}, {}, {}, {}, {})"
    "RETURNING id";

    const std::string INSERT_SITE = 
    "INSERT INTO sim.MonthlySiteData "
    "(MonthlyDataId, LocationId, Population, ClinicalEpisodes, Treatments, TreatmentFailures, EIR, PfPrUnder5, PfPr2to10, PfPrAll) "
    "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {});";

    try {
        // Prepare the summary query
        auto run_id = 0;
        auto days_elapsed = Model::SCHEDULER->current_time();
        auto model_time = std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date);
        auto seasonal_factor = Model::MODEL->get_seasonal_factor(Model::SCHEDULER->calendar_date, 0);
        auto treatment_failures = 0;
        auto beta = 0;
        std::string query = fmt::format(INSERT_COMMON, run_id, days_elapsed, model_time, seasonal_factor, treatment_failures, beta);

        // Run the query and grab the id
        pqxx::work db(*conn);
        pqxx::result result = db.exec(query);
        auto id = result[0][0].as<int>();
        
        // Prepare the site data queries
        query = "";
        for (auto location = 0; location < Model::CONFIG->number_of_locations(); location++) {
            // Determine the EIR
            auto eir = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty() 
                ? 0 : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
            
            query.append(fmt::format(INSERT_SITE,
                id,
                location,
                Model::DATA_COLLECTOR->popsize_by_location()[location],
                Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location],
                Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location],
                -1,
                eir,
                Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) * 100,
                Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) * 100,
                Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] * 100
            ));
        }

        // Run then and commit everything
        db.exec(query);
        db.commit();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

void DbReporter::after_run() {
    // Close the connection 
    conn->close();
}