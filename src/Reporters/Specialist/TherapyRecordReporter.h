/*
 * TherapyRecordReporter.h
 *
 * Define the TherapyRecordReporter class which is used to capture information related to the number of treatments
 * completed each month by location and therapy.
 */
#ifndef THERAPYRECORDREPORTER_H
#define THERAPYRECORDREPORTER_H

#include "SpecialistReporterBase.h"

#include <pqxx/pqxx>

class TherapyRecordReporter : public SpecialistReporterBase {
  private:
    const std::string CHECK_EXISTENCE =
    "SELECT EXISTS (SELECT 1 FROM pg_tables WHERE tablename = 'therapyrecord') AS table_existence";

    const std::string INSERT_THERAPY_PREFIX =
    "INSERT INTO sim.therapyrecord (MonthlyDataId, LocationId, TherapyId, Success, Failure, Completed) VALUES ";

    // To be appended to the INSERT_THERAPY_PREFIX, note that the last character must be replaced with a semicolon
    const std::string INSERT_THERAPY_ROW = "({}, {}, {}, {}, {}, {}),";

    // Aggregate the data by district
    void district_report(int id, pqxx::connection *connection);

  public:
    TherapyRecordReporter() = default;
    ~TherapyRecordReporter() override = default;

    // Basic declarations
    void before_run() override {}
    void after_run() override {}
    void begin_time_step() override {}

    // Overrides
    void initialize(int job_number, std::string path) override;
    void monthly_report() override;
};


#endif
