
/*
 * SQLiteDistrictReporter.h
 *
 * Override the base DbReporter and log genotype information at the district
 * level.
 */
#ifndef SQLITEDISTRICTREPORTER_H
#define SQLITEDISTRICTREPORTER_H

#include "Reporter.h"
#include <sqlite3.h>
#include <string>
#include <vector>

class SQLiteDistrictReporter : public Reporter {
private:
  std::vector<int> district_lookup;
  sqlite3 *db;
  // Execute an SQL statement
  void executeSQL(const std::string &sql);

  template <typename... Args>
  int insertData(const std::string &query, Args... args);

  void monthly_genome_data(int id);
  void monthly_infected_individuals(int id);
  void monthly_site_data(int id);
  void populate_db_schema();
  void populate_genotype_table();
  // Base case
  void bind_values(sqlite3_stmt *) {}

  void bind_values(sqlite3_stmt *stmt, int value) {
    sqlite3_bind_int(stmt, 1, value);
  }

  void bind_values(sqlite3_stmt *stmt, const std::time_t &value) {
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(value));
  }

  void bind_values(sqlite3_stmt *stmt, double value) {
    sqlite3_bind_double(stmt, 3, value);
  }

  template <typename First, typename... Rest>
  void bind_values(sqlite3_stmt *stmt, First first, Rest... rest) {
    bind_values(stmt, first);
    bind_values(stmt, rest...);
  }

private:
  const std::string INSERT_COMMON = R""""(
  INSERT INTO MonthlyData (DaysElapsed, ModelTime, SeasonalFactor)
  VALUES (?, ?, ?)
  RETURNING id;
  )"""";

  const std::string INSERT_SITE_PREFIX = R""""(
    INSERT INTO MonthlySiteData (MonthlyDataId, LocationId, Population, ClinicalEpisodes, Treatments, EIR, PfPrUnder5, PfPr2to10, PfPrAll, TreatmentFailures, NonTreatment) 
    VALUES 
  )"""";

  const std::string INSERT_GENOTYPE_PREFIX = R"""(
    INSERT INTO MonthlyGenomeData 
    (MonthlyDataId, LocationId, GenomeId, Occurrences, 
    ClinicalOccurrences, Occurrences0to5, Occurrences2to10, 
    WeightedOccurrences) 
    VALUES 
  )""";

  const std::string UPDATE_INFECTED_INDIVIDUALS = R"""(
    UPDATE MonthlySiteData SET InfectedIndividuals = {} 
    WHERE MonthlyDataId = {} AND LocationId = {};
  )""";

protected:
  char get_genotype_level() { return 'D'; }

public:
  SQLiteDistrictReporter() = default;
  ~SQLiteDistrictReporter() override;

  void initialize(int job_number, std::string path) override;

  // Basic declarations
  void before_run() override {}
  void begin_time_step() override {}

  // Overrides
  void monthly_report() override;
  void after_run() override;
};

#endif
