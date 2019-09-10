/*
 * DbReporter.h
 * 
 * Define the DbReporter class which is used to insert relevent information from the model into
 * the database during model execution.
 */
#ifndef DBREPORTER_H
#define DBREPORTER_H

#include "Reporter.h"

#include <pqxx/pqxx>

// TODO Add the yaml file name as a field in the configuration table
class DbReporter : public Reporter {
  private:
    const std::string INSERT_COMMON = 
    "INSERT INTO sim.MonthlyData (ReplicateId, DaysElapsed, ModelTime, SeasonalFactor, TreatmentFailures, Beta) "
    "VALUES ({}, {}, {}, {}, {}, {}) RETURNING id;";

    const std::string INSERT_CONFIGURATION = 
    "INSERT INTO sim.Configuration (Yaml, MD5) VALUES ({}, md5({})) RETURNING ID;";

    const std::string INSERT_GENOTYPE = 
    "INSERT INTO sim.MonthlyGenomeData (MonthlyDataId, LocationId, GenomeId, Occurrences, WeightedFrequency) "
    "VALUES ({}, {}, {}, {}, {});";

    const std::string INSERT_LOCATION =
    "INSERT INTO sim.Location (ConfigurationId, Index, Beta) VALUES ({}, {}, {});";

    const std::string INSERT_REPLICATE = 
    "INSERT INTO replicate (ConfigurationId, Seed, Status, Time) VALUES ({}, {}, 0, now()) RETURNING id;";

    const std::string INSERT_SITE = 
    "INSERT INTO sim.MonthlySiteData "
    "(MonthlyDataId, LocationId, Population, ClinicalEpisodes, Treatments, TreatmentFailures, EIR, PfPrUnder5, PfPr2to10, PfPrAll) "
    "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {});";

    const std::string SELECT_CONFIGURATION =
    "SELECT id FROM sim.Configuration WHERE md5 = md5({});";

    const std::string SELECT_LOCATION = 
    "SELECT id, index FROM sim.location WHERE ConfigurationId = {} ORDER BY index;";

    const std::string UPDATE_REPLICATE = 
    "UPDATE replicate SET status = 1 WHERE id = {};";

    // TODO Grab the study id and configuration from... somewhere
    int config_id = 1;

    // TODO Shift this over to where the locations are actually stored?
    int* location_index;
    
    int replicate;
    pqxx::connection* conn;

    // Reporter specific
    void prepare_configuration();
    void prepare_replicate();
    void monthly_genome_data(int id, std::string &query);
    void monthly_site_data(int id, std::string &query);
    
  public:
    DbReporter() = default;
    ~DbReporter() override = default;

    // Basic declarations
    void before_run() override {}
    void begin_time_step() override {}

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;
    void after_run() override;    
};

#endif