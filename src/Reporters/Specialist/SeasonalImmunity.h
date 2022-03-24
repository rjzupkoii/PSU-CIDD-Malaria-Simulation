/*
 * SeasonalImmunity.h
 *
 * This reporter is intended for use at a high level, aggregated by seasonality zone and aggregates the relevant
 * parameters needed to study the interactions between seasonal changes in transmission and immune response.
 */

#ifndef SEASONALIMMUNITY_H
#define SEASONALIMMUNITY_H

#include <vector>

#include "Reporters/Reporter.h"

class SeasonalImmunity : public Reporter {
  private:
    // The lookup to use when mapping a cell to a climatic zone
    std::vector<int> lookup;

    // The maximum value in the lookup table, likely smaller than the size of the lookup
    int lookup_allocation;

    // Build the lookup table that maps the cell to climatic zone
    void build_lookup();

  public:
    SeasonalImmunity() = default;
    ~SeasonalImmunity() override = default;

    // Basic declarations
    void before_run() override {};
    void begin_time_step() override {};
    void after_run() override {};

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;
};

#endif
