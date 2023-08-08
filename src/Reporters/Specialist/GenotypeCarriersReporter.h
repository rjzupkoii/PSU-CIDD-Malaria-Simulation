/*
 * GenotypeCarriersReporter.h
 *
 * This is a specialist reporter class
 */

#ifndef GENOTYPECARRIERSREPORTER_H
#define GENOTYPECARRIERSREPORTER_H

#include "SpecialistReporterBase.h"

#include <pqxx/pqxx>

class GenotypeCarriersReporter : public SpecialistReporterBase {
  private:
    const std::string SELECT_COLUMN =
    "SELECT TRUE FROM pg_attribute "
    "WHERE attrelid = \'sim.monthlysitedata\'::regclass AND attname = \'genotypecarriers\'";

    const std::string UPDATE_GENOTYPES =
    "UPDATE sim.monthlysitedata SET genotypecarriers = {} "
    "WHERE monthlydataid = {} AND location = {}; ";

    // TODO Ideally this should be configurable
    // The genotype we are looking for
    const std::string GENOTYPE = "580Y / 561H";
    const int LOCUS = 2;
    const int ALLELE = 1;

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