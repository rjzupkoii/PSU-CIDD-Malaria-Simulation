/*
 * SpatialData.cpp
 * 
 * Implementation of SpatialData functions.
 */
#include "SpatialData.h"

#include <cmath>
#include <fmt/format.h>
#include <stdexcept>

#include "Core/Config/Config.h"
#include "easylogging++.h"
#include "Model.h"

SpatialData::SpatialData() {
    data = new AscFile*[SpatialFileType::Count]();
}

SpatialData::~SpatialData() {
    if (data == nullptr) { return; }
    for (auto ndx = 0; ndx!= SpatialFileType::Count; ndx++) {
        if (data[ndx] != nullptr) {
            delete data[ndx];
        }
    }
    delete [] data;
}

bool SpatialData::check_catalog(std::string& errors) {
    // Reference parameters
    AscFile* reference;

    // Load the parameters from the first entry
    auto ndx = 0;
    for (; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        reference = data[ndx];
        break;
    }

    // If we hit the end, then there must be nothing loaded
    if (ndx == SpatialFileType::Count) { return true; }

    // Check the remainder of the entries, do this by validating the header
    for (; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        if (data[ndx]->CELLSIZE != reference->CELLSIZE) { errors += "mismatched CELLSIZE;"; }
        if (data[ndx]->NCOLS != reference->NCOLS) { errors += "mismatched NCOLS;"; }
        if (data[ndx]->NODATA_VALUE != reference->NODATA_VALUE) { errors += "mismatched NODATA_VALUE;"; }
        if (data[ndx]->NROWS != reference->NROWS) { errors += "mismatched NROWS;"; }
        if (data[ndx]->XLLCENTER != reference->XLLCENTER) { errors += "mismatched XLLCENTER;"; }
        if (data[ndx]->XLLCORNER != reference->XLLCORNER) { errors += "mismatched XLLCORNER;"; }
        if (data[ndx]->YLLCENTER != reference->YLLCENTER) { errors += "mismatched YLLCENTER;"; }
        if (data[ndx]->YLLCORNER != reference->YLLCORNER) { errors += "mismatched YLLCORNER;"; }
    }

    // Set the dirty flag based upon the errors, return the result
    auto has_errors = !errors.empty();
    dirty = has_errors;
    return has_errors;
}

void SpatialData::generate_distances() const {
    auto& db = Model::CONFIG->location_db();
    auto& distances = Model::CONFIG->spatial_distance_matrix();

    auto locations = db.size();
    distances.resize(static_cast<unsigned long>(locations));
    for (std::size_t from = 0; from < locations; from++) {
        distances[from].resize(static_cast<unsigned long long int>(locations));
        for (std::size_t to = 0; to < locations; to ++) {
            distances[from][to] = std::sqrt(
                std::pow(cell_size * (db[from].coordinate->latitude - db[to].coordinate->latitude), 2) + 
                std::pow(cell_size * (db[from].coordinate->longitude - db[to].coordinate->longitude), 2));
        }
    }

    VLOG(1) << "Updated Euclidean distances using raster data";
}

void SpatialData::generate_locations() {
    // Reference parameters
    AscFile* reference;

    // Scan for an ASC file to use to generate with
    auto ndx = 0;
    for (; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] == nullptr) { continue; }
        reference = data[ndx];
        break;
    }

    // If we didn't find one, return
    if (ndx == SpatialFileType::Count) { 
        LOG(ERROR) << "No spatial file found to generate locations with!";
        return; 
    }

    // Start by over allocating the location_db
    auto& db = Model::CONFIG->location_db();
    db.clear();
    db.reserve(reference->NROWS * reference->NCOLS);

    // Start the id at -1 since we are incrementing it before emplacement
    auto id = -1;

    // Scan the data and insert fields with a value
    for (auto row = 0; row < reference->NROWS; row++) {
        for (auto col = 0; col < reference->NCOLS; col++) {
            if (reference->data[row][col] == reference->NODATA_VALUE) { continue; }
            id++;
            db.emplace_back(id, row, col, 0);
        }
    }

    // It's likely we over allocated, allow some space to be reclaimed
    db.shrink_to_fit();

    // Update the configured count
    Model::CONFIG->number_of_locations.set_value();
    if (Model::CONFIG->number_of_locations() == 0) {
        // This error should be redundant since the ASC loader should catch it
        LOG(ERROR) << "Zero locations loaded while parsing ASC file.";
    }
    VLOG(1) << "Generated " << Model::CONFIG->number_of_locations() << " locations";
}

int SpatialData::get_district(int location) {
    // Throw an error if there are no districts
    if (data[SpatialFileType::Districts] == nullptr) {
        throw std::runtime_error(fmt::format("{} called without district data loaded", __FUNCTION__));
    }

    // Get the coordinate of the location
    auto& coordinate = Model::CONFIG->location_db()[location].coordinate;

    // Use the x, y to get the district id
    auto district = (int)data[SpatialFileType::Districts]->data[(int)coordinate->latitude][(int)coordinate->longitude];
    return district;
}

int SpatialData::get_district_count() {
    // This can be called without throwing an error
    if (data[SpatialFileType::Districts] == nullptr) {
        return -1;
    }

    // Do we already have a count?
    if (district_count != 0) { return district_count; }

    // Perform a consistency check on the districts by first loading all the district IDs while noting the highest value
    std::list<int> districts;
    for (auto ndx = 0; ndx <  data[SpatialFileType::Districts]->NROWS; ndx++) {
        for (auto ndy = 0; ndy <  data[SpatialFileType::Districts]->NCOLS; ndy++) {
            auto value = data[SpatialFileType::Districts]->data[ndx][ndy];
            if (value ==  data[SpatialFileType::Districts]->NODATA_VALUE) { continue; }

            districts.emplace_back(value);
            if (value > static_cast<float>(district_count)) {
                district_count = static_cast<int>(value);
            }
        }
    }

    // Sort the districts and only keep the unique ones, we should have a list from [0, 1] to n with a step of one
    districts.sort();
    districts.unique();

    // Ensure the first value is a zero or one and verify the largest district ID is consistent with the indexing.
    // This check prevents errors when there are gaps in the numbering of the districts or when a one-indexed file
    // contains zeros.
    if (districts.front() == 0) {
      if ((districts.size() - 1) != district_count || (districts.size() - 1) != districts.back()) {
        LOG(ERROR) << "Highest district ID is inconsistent with a zero-based index, array size: " << districts.size() << ", last ID value: " << districts.back();
        throw std::invalid_argument("District raster inconsistently numbered, or contains invalid data.");
      }
      // Add a notice to the file since zero-based indexing for ASC files would be unusual
      LOG(INFO) << "File suggests zero-based district numbering.";
      first_district = 0;
    } else if (districts.front() == 1) {
      if (districts.size() != district_count || districts.size() != districts.back()) {
        LOG(ERROR) << "Highest district ID is inconsistent with a one-based index, expected " << districts.size() << ", got " << districts.back();
        throw std::invalid_argument("District raster inconsistently numbered, or contains invalid data.");
      }
      first_district = 1;
    } else {
      LOG(ERROR) << "Index of first district must be zero or one, found " << districts.front();
      throw std::invalid_argument("District raster must be zero-based or one-based.");
    }

    // Return the result
    return district_count;
}

std::vector<int> SpatialData::get_district_locations(int district) {
  // Throw an error if there are no districts
  if (data[SpatialFileType::Districts] == nullptr) {
    throw std::runtime_error(fmt::format("{} called without district data loaded", __FUNCTION__));
  }

  // Prepare our data
  std::vector<int> locations;
  AscFile* reference = data[SpatialFileType::Districts];

  // Scan the district raster and use it to generate the location ids, the logic here is the same
  // as the generation of the location ids in generate_locations
  auto id = -1;
  for (auto ndx = 0; ndx < reference->NROWS; ndx++) {
    for (auto ndy = 0; ndy < reference->NCOLS; ndy++) {
      if (reference->data[ndx][ndy] == reference->NODATA_VALUE) { continue; }
      id++;
      if ((int)reference->data[ndx][ndy] == district) {
        locations.emplace_back(id);
      }
    }
  }

  // Return the results
  return locations;
}

int SpatialData::get_first_district() {
  // Do we already have a value, implied by a district count
  if (district_count != 0) {
    return first_district;
  }

  // Generate the district count and first district index
  get_district_count();
  return first_district;
}

SpatialData::RasterInformation SpatialData::get_raster_header() {
    RasterInformation results;
    for (auto ndx = 0; ndx < SpatialFileType::Count; ndx++) {
        if (data[ndx] != nullptr) {
            results.number_columns = data[ndx]->NCOLS;
            results.number_rows = data[ndx]->NROWS;
            results.x_lower_left_corner = data[ndx]->XLLCORNER;
            results.y_lower_left_corner = data[ndx]->YLLCORNER;
            results.cellsize = data[ndx]->CELLSIZE;
            break;
        }
    }
    return results;
}

bool SpatialData::has_raster() {
    for (auto ndx = 0; ndx != SpatialFileType::Count; ndx++) {
        if (data[ndx] != nullptr) { return true; }
    }
    return false;
}

void SpatialData::load(const std::string& filename, SpatialFileType type) {
    // Check to see if something has already been loaded
    if (data[type] != nullptr) { delete data[type]; }

    // Load the data and set the reference
    VLOG(1) << "Loading " << filename;
    AscFile* file = AscFileManager::read(filename);
    data[type] = file;

    // The data needs to be checked
    dirty = true;
}

void SpatialData::load_raster(SpatialFileType type) {
    // Verify that the raster data has been loaded
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("{} called without raster, type id: {}", __FUNCTION__, type));
    }
    auto *values = data[type];

    // Grab a reference to the location_db to work with
    auto& location_db = Model::CONFIG->location_db();
    auto count = Model::CONFIG->number_of_locations();    

    // Iterate through the raster and locations to set the value
    auto id = 0;
    for (auto row = 0; row < values->NROWS; row++) {
        for (auto col = 0; col < values->NCOLS; col++) {
            if (values->data[row][col] == values->NODATA_VALUE) { continue; }

            if (id > count) {
                throw std::runtime_error(fmt::format("Raster misalignment: pixel count exceeds {} expected while loading raster type {}", count, type));
            }

            switch (type) {
                case SpatialFileType::Beta: 
                    location_db[id].beta = values->data[row][col]; 
                    break;
                case SpatialFileType::Population:
                    // Verify that we aren't losing data
                    if (values->data[row][col] != ceil((double)values->data[row][col])) {
                        LOG(WARNING) << fmt::format("Population data lost at row {}, col {}, value {}", row, col, values->data[row][col]);
                    }
                    location_db[id].population_size = static_cast<int>(values->data[row][col]);
                    break;
                case SpatialFileType::PrTreatmentUnder5: 
                    location_db[id].p_treatment_less_than_5 = values->data[row][col]; 
                    break;
                case SpatialFileType::PrTreatmentOver5: 
                    location_db[id].p_treatment_more_than_5 = values->data[row][col]; 
                    break;
                default: 
                    throw std::runtime_error(fmt::format("{} called with invalid raster, type id: {}", __FUNCTION__, type));
            }
            id++;
        }
    }

    // When we are done the last id value should match the number of locations, accounting for zero indexing
    if ((unsigned)(id) != count) {
        throw std::runtime_error(fmt::format("Raster misalignment: found {} pixels, expected {} while loading raster type {}", id, count, type));
    }

    // Log the updates
    VLOG(1) << "Loaded values from raster file, type id: " << type;
}

void SpatialData::load_files(const YAML::Node &node) {
    if (node[LOCATION_RASTER]) {
        load(node[LOCATION_RASTER].as<std::string>(), SpatialData::SpatialFileType::Locations);
    }
    if (node[BETA_RASTER]) {
        load(node[BETA_RASTER].as<std::string>(), SpatialData::SpatialFileType::Beta);
    }
    if (node[POPULATION_RASTER]) {
        load(node[POPULATION_RASTER].as<std::string>(), SpatialData::SpatialFileType::Population);
    }
    if (node[DISTRICT_RASTER]) {
        load(node[DISTRICT_RASTER].as<std::string>(), SpatialData::SpatialFileType::Districts);
    }
    if (node[TRAVEL_RASTER]) {
        load(node[TRAVEL_RASTER].as<std::string>(), SpatialData::SpatialFileType::Travel);
    }
    if (node[ECOCLIMATIC_RASTER]) {
        load(node[ECOCLIMATIC_RASTER].as<std::string>(), SpatialData::SpatialFileType::Ecoclimatic);
    }
    if (node[TREATMENT_RATE_UNDER5]) {
        load(node[TREATMENT_RATE_UNDER5].as<std::string>(), SpatialData::SpatialFileType::PrTreatmentUnder5);
    }
    if (node[TREATMENT_RATE_OVER5]) {
        load(node[TREATMENT_RATE_OVER5].as<std::string>(), SpatialData::SpatialFileType::PrTreatmentOver5);
    }
}

bool SpatialData::parse(const YAML::Node &node) {

    // First, start by attempting to load any rasters
    load_files(node);

    // Set the cell size
    cell_size = node["cell_size"].as<float>();

    // Now convert the rasters into the location space
    refresh();

    // Grab a reference to the location_db to work with
    auto& location_db = Model::CONFIG->location_db();
    auto number_of_locations = Model::CONFIG->number_of_locations();

    // Load the age distribution from the YAML
    for (auto loc = 0ul; loc < number_of_locations; loc++) {
        auto input_loc = node["age_distribution_by_location"].size() < number_of_locations ? 0 : loc;
        for (auto i = 0ul; i < node["age_distribution_by_location"][input_loc].size(); i++) {
            location_db[loc].age_distribution.push_back(node["age_distribution_by_location"][input_loc][i].as<double>());
        }
    }

    // Load the treatment information, note that if rasters were loaded via refresh() this will end up being a NOP
    for (auto loc = 0ul; loc < number_of_locations; loc++) {
        if (!node[TREATMENT_RATE_UNDER5]) {
            // Set the under 5 value
            auto input_loc = node["p_treatment_for_less_than_5_by_location"].size() < number_of_locations ? 0 : loc;
            location_db[loc].p_treatment_less_than_5 = node["p_treatment_for_less_than_5_by_location"][input_loc].as<float>();
        }
        if (!node[TREATMENT_RATE_OVER5]) {
            // Set the over 5 value
            auto input_loc = node["p_treatment_for_more_than_5_by_location"].size() < number_of_locations ? 0 : loc;
            location_db[loc].p_treatment_more_than_5 = node["p_treatment_for_more_than_5_by_location"][input_loc].as<float>();
        }
    }

    // Load the beta if we don't have a raster for it
    if (!node[BETA_RASTER]) {
        for (auto loc = 0ul; loc < number_of_locations; loc++) {
            auto input_loc = node["beta_by_location"].size() < number_of_locations ? 0 : loc;
            location_db[loc].beta = node["beta_by_location"][input_loc].as<float>();
        }
    }

    // Load the population if we don't have a raster for it
    if (!node[POPULATION_RASTER]) {
        for (auto loc = 0ul; loc < number_of_locations; loc++) {
            auto input_loc = node["population_size_by_location"].size() < number_of_locations ? 0 : loc;
            location_db[loc].population_size = node["population_size_by_location"][input_loc].as<int>();
        }
    }

    // Load completed successfully
    parse_complete();
    return true;
}

void SpatialData::parse_complete() {
    // Delete redundant rasters once they have been loaded to the location_db
    if (data[SpatialFileType::Beta] != nullptr) {
        delete data[SpatialFileType::Beta];
        data[SpatialFileType::Beta] = nullptr;
    }
    if (data[SpatialFileType::Population] != nullptr) {
        delete data[SpatialFileType::Population];
        data[SpatialFileType::Population] = nullptr;
    }
    if (data[SpatialFileType::PrTreatmentUnder5] != nullptr) {
        delete data[SpatialFileType::PrTreatmentUnder5];
        data[SpatialFileType::PrTreatmentUnder5] = nullptr;
    }
    if (data[SpatialFileType::PrTreatmentOver5] != nullptr) {
        delete data[SpatialFileType::PrTreatmentOver5];
        data[SpatialFileType::PrTreatmentOver5] = nullptr;
    }
}

void SpatialData::refresh() {
    // Check to make sure our data is OK
    std::string errors;
    if (dirty && check_catalog(errors)) {
        throw std::runtime_error(errors);
    }

    // We have data, and we know that it should be located in the same geographic space, so now we can now refresh the location_db
    if (Model::CONFIG->number_of_locations() == 0) {
        generate_locations();
    }
    
    // Load the remaining data, note this spot is a marker for adding more types
    if (data[SpatialFileType::Beta] != nullptr) { load_raster(SpatialFileType::Beta); }
    if (data[SpatialFileType::Population] != nullptr) { load_raster(SpatialFileType::Population); }
    if (data[SpatialFileType::PrTreatmentUnder5] != nullptr) { load_raster(SpatialFileType::PrTreatmentUnder5); }
    if (data[SpatialFileType::PrTreatmentOver5] != nullptr) { load_raster(SpatialFileType::PrTreatmentOver5); }
}

void SpatialData::write(const std::string& filename, SpatialFileType type) {
    // Check to make sure there is something to write
    if (data[type] == nullptr) {
        throw std::runtime_error(fmt::format("No data for spatial file type {}, write file {}", type, filename)); 
    }

    // Write the data
    AscFileManager::write(data[type], filename);
}