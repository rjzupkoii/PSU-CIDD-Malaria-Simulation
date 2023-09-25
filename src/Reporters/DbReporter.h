/*
 * DbReporter.h
 * 
 * Define the DbReporter class which is used to insert relevant information from the model into
 * the database during model execution.
 */
#ifndef DBREPORTER_H
#define DBREPORTER_H

#include "Reporter.h"

#include <pqxx/pqxx>

class DbReporter : public Reporter {
  private:
    const std::string INSERT_COMMON = 
    "INSERT INTO sim.MonthlyData (ReplicateId, DaysElapsed, ModelTime, SeasonalFactor, EntryTime) "
    "VALUES ({}, {}, {}, {}, now()) RETURNING id;";

    const std::string INSERT_CONFIGURATION = 
    "INSERT INTO sim.Configuration (Yaml, MD5, FileName) VALUES ({}, md5({}), {}) RETURNING ID;";

    const std::string INSERT_CONFIGURATION_RASTER =
    "INSERT INTO sim.Configuration (Yaml, MD5, FileName, ncols, nrows, xllcorner, yllcorner, cellsize) "
    "VALUES ({}, md5({}), {}, {}, {}, {}, {}, {}) RETURNING ID;";

    const std::string INSERT_LOCATION =
    "INSERT INTO sim.Location (ConfigurationId, Index, X, Y, Beta) VALUES ({}, {}, {}, {}, {});";

    const std::string INSERT_LOCATION_DISTRICT = 
    "INSERT INTO sim.Location (ConfigurationId, Index, X, Y, Beta, District) VALUES ({}, {}, {}, {}, {}, {});";

    const std::string INSERT_REPLICATE = 
    "INSERT INTO replicate (ConfigurationId, Seed, StartTime, Movement, AggregationLevel) VALUES ({}, {}, now(), '{}', '{}') RETURNING id;";

    const std::string SELECT_CONFIGURATION =
    "SELECT id FROM sim.Configuration WHERE md5 = md5({}) AND filename = {};";

    const std::string SELECT_LOCATION = 
    "SELECT id, index FROM sim.location WHERE ConfigurationId = {} ORDER BY index;";

    const std::string UPDATE_REPLICATE = 
    "UPDATE replicate SET EndTime = now() WHERE id = {};";

    const std::string UPDATE_CONFIGURATION_STUDY =
    "UPDATE configuration SET studyid = {} WHERE id = {}";

    // Number of times various retry events should be tried
    int RETRY_LIMIT = 10;

    // Time to wait between trying to reconnect to the database, 10 seconds in milliseconds
    int WAIT_TIMESPAN = 10000;

    // Configuration and replicate information
    int config_id = 1;
    int replicate;

    // Prepare a connection to the database, pointer returned must be deleted
    pqxx::connection* get_connection() const;

    // Reporter specific
    bool do_monthly_report();
    void update_infected_individuals(int id, std::string &query);
    void prepare_configuration(pqxx::connection* connection);
    void prepare_replicate(pqxx::connection* connection);

  protected:
    const std::string INSERT_GENOTYPE_PREFIX =
    "INSERT INTO sim.MonthlyGenomeData (MonthlyDataId, Location, GenomeId, Occurrences, ClinicalOccurrences, Occurrences0to5, Occurrences2to10, WeightedOccurrences) VALUES ";

    // To be appended to INSERT_GENOTYPE_PREFIX, note that the last character must be replaced with a semicolon
    const std::string INSERT_GENOTYPE_ROW = "({}, {}, {}, {}, {}, {}, {}, {}),";

    const std::string INSERT_SITE =
    "INSERT INTO sim.MonthlySiteData "
    "(MonthlyDataId, Location, Population, ClinicalEpisodes, Treatments, EIR, PfPrUnder5, PfPr2to10, PfPrAll, TreatmentFailures, NonTreatment, under5treatment, over5treatment) "
    "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {});";

    const std::string UPDATE_INFECTED_INDIVIDUALS =
    "UPDATE sim.MonthlySiteData SET InfectedIndividuals = {} WHERE MonthlyDataId = {} AND Location = {};";

    // Used to track location information for the life of the object
    std::vector<int> location_index;

    // Return the character code that indicates the level of genotype records (c: cell, d: district)
    virtual char get_genotype_level() { return 'C'; }

    virtual void monthly_genome_data(int id, std::string &query);
    virtual void monthly_site_data(int id, std::string &query);

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