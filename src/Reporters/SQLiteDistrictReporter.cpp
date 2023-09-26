#include "SQLiteDistrictReporter.h"
#include "Core/Config/Config.h"
#include "GIS/SpatialData.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "easylogging++.h"

// Helper function to join the strings in a vector with a delimiter
std::string join(const std::vector<std::string> &vec,
                 const std::string &delimiter) {
  std::string result;
  for (size_t i = 0; i < vec.size(); i++) {
    result += vec[i];
    if (i != vec.size() - 1) {
      result += delimiter;
    }
  }
  return result;
}

SQLiteDistrictReporter::~SQLiteDistrictReporter() {
  if (db) {
    sqlite3_close(db);
  }
}

void SQLiteDistrictReporter::populate_genotype_table() {
  const std::string INSERT_GENOTYPE =
      "INSERT INTO genotype (id, name) VALUES (?, ?);";

  try {
    // Prepare the bulk query
    std::string query = "DELETE FROM genotype;";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
      // Handle the error
      LOG(ERROR) << "Error preparing DELETE statement: " << sqlite3_errmsg(db);
    }
    rc = sqlite3_step(stmt); // Execute the DELETE statement
    if (rc != SQLITE_DONE) {
      // Handle the error
      LOG(ERROR) << "Error executing DELETE statement: " << sqlite3_errmsg(db);
    }
    sqlite3_finalize(stmt); // Finalize the statement

    stmt = nullptr; // Reset the statement

    // Now, insert the data
    rc = sqlite3_prepare_v2(db, INSERT_GENOTYPE.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
      // Handle the error
      LOG(ERROR) << "Error preparing INSERT statement: " << sqlite3_errmsg(db);
    }

    auto *config = Model::CONFIG;

    for (auto id = 0ul; id < config->number_of_parasite_types(); id++) {
      auto genotype = (*config->genotype_db())[id];
      // Bind values to the prepared statement
      sqlite3_bind_int(stmt, 1, id);
      sqlite3_bind_text(stmt, 2, genotype->to_string(config).c_str(), -1,
                        SQLITE_STATIC);

      rc = sqlite3_step(stmt); // Execute the INSERT statement
      if (rc != SQLITE_DONE) {
        // Handle the error
        LOG(ERROR) << "Error executing INSERT statement: "
                   << sqlite3_errmsg(db);
      }
      sqlite3_reset(stmt); // Reset the statement for the next iteration
    }

    sqlite3_finalize(stmt); // Finalize the statement

  } catch (const std::exception &ex) {
    LOG(ERROR) << __FUNCTION__ << "-" << ex.what();
    exit(1);
  }
}

void SQLiteDistrictReporter::executeSQL(const std::string &sql) {
  char *zErrMsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
  if (rc != SQLITE_OK) {
    LOG(ERROR) << "SQL error: " << zErrMsg;
    sqlite3_free(zErrMsg);
  }
}

void SQLiteDistrictReporter::populate_db_schema() {

  // Create the table schema
  /* executeSQL(R""""( */
  /*   CREATE TABLE IF NOT EXISTS location ( */
  /*       id INTEGER PRIMARY KEY AUTOINCREMENT, */
  /*       location_index INTEGER NOT NULL, */
  /*       x INTEGER NOT NULL, */
  /*       y INTEGER NOT NULL, */
  /*       beta REAL NOT NULL, */
  /*       district INTEGER, */
  /*       CONSTRAINT location_index_unique UNIQUE (id, location_index) */
  /*   ); */
  /* )""""); */
  /**/
  executeSQL(R""""(
    CREATE TABLE IF NOT EXISTS monthlydata (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        dayselapsed INTEGER NOT NULL,
        modeltime INTEGER NOT NULL,
        seasonalfactor INTEGER NOT NULL,
    );
  )"""");

  executeSQL(R""""(
    CREATE TABLE IF NOT EXISTS monthlysitedata (
        monthlydataid INTEGER NOT NULL,
        locationid INTEGER NOT NULL,
        population INTEGER NOT NULL,
        clinicalepisodes INTEGER NOT NULL,
        treatments INTEGER NOT NULL,
        treatmentfailures INTEGER NOT NULL,
        eir REAL NOT NULL,
        pfprunder5 REAL NOT NULL,
        pfpr2to10 REAL NOT NULL,
        pfprall REAL NOT NULL,
        infectedindividuals INTEGER,
        nontreatment INTEGER NOT NULL,
        PRIMARY KEY (monthlydataid, locationid),
        FOREIGN KEY (monthlydataid) REFERENCES monthlydata(id)
    );
  )"""");

  executeSQL(R""""(
    CREATE TABLE IF NOT EXISTS genotype (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL
    );
  )"""");

  executeSQL(R""""(
    CREATE TABLE IF NOT EXISTS monthlygenomedata (
        monthlydataid INTEGER NOT NULL,
        locationid INTEGER NOT NULL,
        genomeid INTEGER NOT NULL,
        occurrences INTEGER NOT NULL,
        clinicaloccurrences INTEGER NOT NULL,
        occurrences0to5 INTEGER NOT NULL,
        occurrences2to10 INTEGER NOT NULL,
        weightedoccurrences REAL NOT NULL,
        PRIMARY KEY (monthlydataid, genomeid, locationid),
        FOREIGN KEY (genomeid) REFERENCES genotype(id),
        FOREIGN KEY (monthlydataid) REFERENCES monthlydata(id)
    );
  )"""");
}

void SQLiteDistrictReporter::initialize(int job_number, std::string path) {
  // Inform the user of the reporter type and make sure there are districts
  VLOG(1)
      << "Using SQLiteDistrictReporterwith aggregation at the district level.";
  if (!SpatialData::get_instance().has_raster(SpatialData::Districts)) {
    LOG(ERROR) << "District raster must be present when aggregating data at "
                  "the district level.";
    throw std::invalid_argument("No district raster present");
  }

  // Build a lookup for location to district
  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
    district_lookup.emplace_back(SpatialData::get_instance().get_district(loc));
  }

  // Define the database file path
  auto dbPath = fmt::format("{}monthly_data_{}.db", path, job_number);

  // Check if the file exists
  if (std::filesystem::exists(dbPath)) {
    // Delete the old database file if it exists
    if (std::remove(dbPath.c_str()) != 0) {
      // Handle the error, if any, when deleting the old file
      LOG(ERROR) << "Error deleting old database file.";
    }
  } else {
    // The file doesn't exist, so no need to delete it
    LOG(INFO) << "Database file does not exist. No deletion needed."
              << std::endl;
  }
  int result;

  // Open or create the SQLite database file
  result = sqlite3_open_v2(dbPath.c_str(), &db,
                           SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
  if (result != SQLITE_OK) {
    // Handle the error (e.g., throw an exception)
    LOG(ERROR) << "Error opening SQLite database: " << sqlite3_errmsg(db);
  }

  populate_db_schema();
  // populate the genotype table
  populate_genotype_table();
}

template <typename... Args>
int SQLiteDistrictReporter::insertData(const std::string &query, Args... args) {
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    LOG(ERROR) << "Failed to prepare statement: " << sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    exit(1);
  }

  bind_values(stmt, args...);
  int step_result = sqlite3_step(stmt);
  if (step_result == SQLITE_ROW) {
    int returned_id = sqlite3_column_int(stmt, 0);

    // Expect the next step to be SQLITE_DONE
    if (sqlite3_step(stmt) != SQLITE_DONE) {
      LOG(ERROR) << "Execution didn't finish: " << sqlite3_errmsg(db);
      sqlite3_finalize(stmt);
      exit(1);
    }

    sqlite3_finalize(stmt);
    return returned_id;
  } else if (step_result != SQLITE_DONE) {
    LOG(ERROR) << "Execution didn't finish: " << sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    exit(1);
  }

  sqlite3_finalize(stmt);
  return -1; // In case no ID is returned, but this shouldn't happen
}

void SQLiteDistrictReporter::monthly_report() {

  // Get the relevant data
  auto days_elapsed = Model::SCHEDULER->current_time();
  auto model_time =
      std::chrono::system_clock::to_time_t(Model::SCHEDULER->calendar_date);
  auto seasonal_factor = Model::CONFIG->seasonal_info()->get_seasonal_factor(
      Model::SCHEDULER->calendar_date, 0);

  auto id =
      insertData(INSERT_COMMON, days_elapsed, model_time, seasonal_factor);

  std::string query = "";
  monthly_site_data(id);
  if (Model::CONFIG->record_genome_db() &&
      Model::DATA_COLLECTOR->recording_data()) {
    // Add the genome information, this will also update infected individuals
    monthly_genome_data(id);
  } else {
    // If we aren't recording genome data still update the infected
    // individuals
    monthly_infected_individuals(id);
  }
}

void SQLiteDistrictReporter::after_run() {
  if (db) {
    sqlite3_close(db);
    db = nullptr;
  }
}

void SQLiteDistrictReporter::monthly_site_data(int id) {
  // Prepare the data structures
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  std::vector<double> eir(districts, 0);
  std::vector<double> pfpr_under5(districts, 0);
  std::vector<double> pfpr_2to10(districts, 0);
  std::vector<double> pfpr_all(districts, 0);
  std::vector<int> population(districts, 0);
  std::vector<int> clinical_episodes(districts, 0);
  std::vector<int> treatments(districts, 0);
  std::vector<int> treatment_failures(districts, 0);
  std::vector<int> nontreatment(districts, 0);

  // Collect the data
  for (auto location = 0; location < Model::CONFIG->number_of_locations();
       location++) {

    // If the population is zero, press on
    auto location_population =
        Model::DATA_COLLECTOR->popsize_by_location()[location];
    if (location_population == 0) {
      continue;
    }

    // Note the district we are in, make sure things are zero indexed
    auto district = district_lookup[location] - first_index;

    // Collect the simple data
    population[district] += location_population;
    clinical_episodes[district] +=
        Model::DATA_COLLECTOR
            ->monthly_number_of_clinical_episode_by_location()[location];
    treatments[district] +=
        Model::DATA_COLLECTOR
            ->monthly_number_of_treatment_by_location()[location];
    treatment_failures[district] +=
        Model::DATA_COLLECTOR
            ->monthly_treatment_failure_by_location()[location];
    nontreatment[district] +=
        Model::DATA_COLLECTOR->monthly_nontreatment_by_location()[location];

    // EIR and PfPR is a bit more complicated since it could be an invalid value
    // early in the simulation, and when aggregating at the district level the
    // weighted mean needs to be reported instead
    if (Model::DATA_COLLECTOR->recording_data()) {
      auto eir_location =
          Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()
              ? 0
              : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
      eir[district] += (eir_location * location_population);
      pfpr_under5[district] +=
          (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 0, 5) *
           location_population);
      pfpr_2to10[district] +=
          (Model::DATA_COLLECTOR->get_blood_slide_prevalence(location, 2, 10) *
           location_population);
      pfpr_all[district] +=
          (Model::DATA_COLLECTOR
               ->blood_slide_prevalence_by_location()[location] *
           location_population);
    }
  }

  std::vector<std::string> values;

  for (auto district = 0; district < districts; district++) {
    double calculatedEir =
        (eir[district] != 0) ? (eir[district] / population[district]) : 0;
    double calculatedPfprUnder5 =
        (pfpr_under5[district] != 0)
            ? (pfpr_under5[district] / population[district]) * 100.0
            : 0;
    double calculatedPfpr2to10 =
        (pfpr_2to10[district] != 0)
            ? (pfpr_2to10[district] / population[district]) * 100.0
            : 0;
    double calculatedPfprAll =
        (pfpr_all[district] != 0)
            ? (pfpr_all[district] / population[district]) * 100.0
            : 0;

    std::string singleRow = fmt::format(
        "({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {})", id,
        district + first_index, population[district],
        clinical_episodes[district], treatments[district], calculatedEir,
        calculatedPfprUnder5, calculatedPfpr2to10, calculatedPfprAll,
        treatment_failures[district], nontreatment[district]);
    values.push_back(singleRow);
  }

  std::string query = INSERT_SITE_PREFIX + join(values, ",") + ";";
  executeSQL(query);
}

void SQLiteDistrictReporter::monthly_genome_data(int id) {

  // Cache some values
  auto genotypes = Model::CONFIG->number_of_parasite_types();
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  auto *index =
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = index->vPerson()[0][0].size();

  // Prepare the data structures
  std::vector<int> individual(genotypes, 0);
  std::vector<int> infections_district(districts, 0);
  std::vector<std::vector<int>> occurrences(districts,
                                            std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> clinicalOccurrences(
      districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> occurrencesZeroToFive(
      districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<int>> occurrencesTwoToTen(
      districts, std::vector<int>(genotypes, 0));
  std::vector<std::vector<double>> weightedOccurrences(
      districts, std::vector<double>(genotypes, 0));

  // Iterate over all the possible states
  for (auto location = 0; location < index->vPerson().size(); location++) {

    // Get the current index and apply the off set, so we are zero aligned
    auto district = district_lookup[location] - first_index;
    int infectedIndividuals = 0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      // Iterate over all the age classes
      for (unsigned int ac = 0; ac < age_classes; ac++) {
        // Iterate over all the genotypes
        auto age_class = index->vPerson()[location][hs][ac];
        for (auto &person : age_class) {

          // Get the person, press on if they are not infected
          auto parasites =
              person->all_clonal_parasite_populations()->parasites();
          auto size = parasites->size();
          if (size == 0) {
            continue;
          }

          // Note the age and clinical status of the person
          auto age = person->age();
          auto clinical =
              (int)(person->host_state() == Person::HostStates::CLINICAL);

          // Update the count of infected individuals
          infectedIndividuals++;

          // Count the genotypes present in the individual
          for (unsigned int ndx = 0; ndx < size; ndx++) {
            auto parasite_population = (*parasites)[ndx];
            auto genotype_id = parasite_population->genotype()->genotype_id();
            occurrences[district][genotype_id]++;
            occurrencesZeroToFive[district][genotype_id] += (age <= 5);
            occurrencesTwoToTen[district][genotype_id] +=
                (age >= 2 && age <= 10);
            individual[genotype_id]++;

            // Count a clinical occurrence if the individual has clinical
            // symptoms
            clinicalOccurrences[district][genotype_id] += clinical;
          }

          // Update the weighted occurrences and reset the individual count
          for (unsigned int ndx = 0; ndx < genotypes; ndx++) {
            weightedOccurrences[district][ndx] +=
                (individual[ndx] / static_cast<double>(size));
            individual[ndx] = 0;
          }
        }
      }
    }

    // Update the number of individuals in the location
    infections_district[district] += infectedIndividuals;
  }

  std::vector<std::string> insertValues;

  // Iterate over the districts and append the query
  std::string insert_genotypes, update_infections;
  for (auto district = 0; district < districts; district++) {
    for (auto genotype = 0; genotype < genotypes; genotype++) {
      if (weightedOccurrences[district][genotype] == 0) {
        continue;
      }
      std::string singleRow = fmt::format(
          "({}, {}, {}, {}, {}, {}, {}, {})", id, district + first_index,
          genotype, occurrences[district][genotype],
          clinicalOccurrences[district][genotype],
          occurrencesZeroToFive[district][genotype],
          occurrencesTwoToTen[district][genotype],
          weightedOccurrences[district][genotype]);

      insertValues.push_back(singleRow);
    }

    std::string updateQuery =
        fmt::format(UPDATE_INFECTED_INDIVIDUALS, infections_district[district],
                    id, district + first_index);
    executeSQL(updateQuery);
  }
  if (insertValues.empty()) {
    LOG(INFO) << "No genotypes recorded in the simulation at timestep, "
              << Model::SCHEDULER->current_time();
    return;
  }

  std::string insertQuery =
      INSERT_GENOTYPE_PREFIX + join(insertValues, ",") + ";";
  executeSQL(insertQuery);
}

void SQLiteDistrictReporter::monthly_infected_individuals(int id) {
  // Cache some values and prepare the data structure
  auto districts = SpatialData::get_instance().get_district_count();
  auto first_index = SpatialData::get_instance().get_first_district();
  auto *index =
      Model::POPULATION->get_person_index<PersonIndexByLocationStateAgeClass>();
  auto age_classes = index->vPerson()[0][0].size();
  std::vector<int> infections_district(districts, 0);

  // Iterate overall the possible locations and states
  for (auto location = 0; location < index->vPerson().size(); location++) {
    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (unsigned int ac = 0; ac < age_classes; ac++) {
        for (auto &person : index->vPerson()[location][hs][ac]) {
          // Is the individual infected by at least one parasite?
          if (person->all_clonal_parasite_populations()->parasites()->empty()) {
            continue;
          }

          // Calculate the correct index and update the count
          auto district = district_lookup[location] - first_index;
          infections_district[district]++;
        }
      }
    }
  }

  const std::string UPDATE_INFECTED_INDIVIDUALS = R"""(
    UPDATE MonthlySiteData SET InfectedIndividuals = {} 
    WHERE MonthlyDataId = {} AND LocationId = {};
  )""";
  // Iterate over the districts and append the query
  for (auto district = 0; district < districts; district++) {
    std::string updateQuery =
        fmt::format(UPDATE_INFECTED_INDIVIDUALS, infections_district[district],
                    id, district + first_index);
    executeSQL(updateQuery);
  }
}
