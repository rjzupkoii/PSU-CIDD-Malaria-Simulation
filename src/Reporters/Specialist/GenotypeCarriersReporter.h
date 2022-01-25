/*
 * GenotypeCarriersReporter.h
 *
 * This is a specialist reporter class
 */

#ifndef GENOTYPECARRIERSREPORTER_H
#define GENOTYPECARRIERSREPORTER_H

#include "Reporters/Reporter.h"

#include <pqxx/pqxx>

class GenotypeCarriersReporter : public Reporter {
  private:
    const std::string SELECT_AGGREGATION =
    "SELECT aggregationlevel FROM sim.replicate "
    "WHERE ID = {} AND endtime IS NULL";

    const std::string SELECT_COLUMN =
    "SELECT TRUE FROM pg_attribute "
    "WHERE attrelid = \'sim.monthlysitedata\'::regclass AND attname = \'genotypecarriers\'";

    const std::string SELECT_LOCATION =
    "SELECT l.id, l.index "
    "FROM sim.replicate r INNER JOIN sim.location l ON l.configurationid = r.configurationid "
    "WHERE r.id = {} ORDER BY INDEX";

    const std::string SELECT_MONTHLYDATAID =
    "SELECT id FROM sim.monthlydata "
    "WHERE replicateid = {} ORDER BY dayselapsed DESC LIMIT 1";

    const std::string UPDATE_GENOTYPES =
    "UPDATE sim.monthlysitedata SET genotypecarriers = {} "
    "WHERE monthlydataid = {} AND location = {}; ";

    // TODO Ideally this should be configurable
    // The genotype we are looking for
    const std::string GENOTYPE = "580Y / 561H";
    const int LOCUS = 2;
    const int ALLELE = 1;

    // The aggregation level used in reporting
    std::string aggregation;

    // The lookup to use when mapping cell to district, will be one-to-one if cellular aggregation is used
    std::vector<int> lookup;

    // The maximum value in the lookup table, may not be the same as the size if we are operating at the district level
    int lookup_allocation;

    // Build the lookup table that maps from cell to district, or cell to cell for reporting
    void build_lookup(int replicate, pqxx::connection* connection);

  public:
    GenotypeCarriersReporter() = default;
    ~GenotypeCarriersReporter() override = default;

    // Basic declarations
    void before_run() override {}
    void after_run() override {}
    void begin_time_step() override {}

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;
};

#endif