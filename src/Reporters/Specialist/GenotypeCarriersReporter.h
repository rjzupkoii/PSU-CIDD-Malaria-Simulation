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
    const std::string SELECT_COLUMN =
    "SELECT TRUE FROM pg_attribute "
    "WHERE attrelid = \'sim.monthlysitedata\'::regclass AND attname = \'genotypecarriers\'";

    const std::string SELECT_AGGREGATION =
    "SELECT aggregationlevel FROM sim.replicate "
    "WHERE ID = {} AND endtime IS NULL";

    // TODO Ideally this should be configurable
    // The genotype we are looking for
    const std::string GENOTYPE = "561H";

    // The aggregation level to use when recording counts, retrieved from database
    std::string aggregation;

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