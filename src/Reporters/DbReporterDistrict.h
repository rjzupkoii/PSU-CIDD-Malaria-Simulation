/*
 * DbReporterDistrict.h
 *
 * Override the base DbReporter and log genotype information at the district level.
 */
#ifndef DBREPORTERDISTRICT_H
#define DBREPORTERDISTRICT_H

#include "DbReporter.h"

class DbReporterDistrict : public DbReporter {
  private:
    std::vector<int> district_lookup;

  protected:
    char get_genotype_level() override { return 'D'; }

    void monthly_genome_data(int id, std::string &query) override;
    void monthly_site_data(int id, std::string &query) override;

  public:
    void initialize(int job_number, std::string path) override;
};

#endif
