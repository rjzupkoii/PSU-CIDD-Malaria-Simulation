/**
 * IntroduceMutantEventBuilder.cpp
 *
 * This file contains the implementation details for events that inherit from the IntroduceMutantEventBase class.
*/
#include "Events/Population/PopulationEventBuilder.h"

#include "Core/Config/Config.h"
#include "Events/Population/IntroduceMutantEvent.hxx"
#include "Events/Population/IntroduceMutantRasterEvent.hxx"
#include "GIS/AscFile.h"
#include "GIS/SpatialData.h"
#include "Model.h"

// Parse the file provided and return the location ids from it.
//
// NOTE That this is a more in line with the C way of doing things, but we are keeping this logic isolated to just where
//      it will be used as opposed to having it somewhere else.
std::vector<int> get_locations_from_raster(const std::string& filename) {
  // Load the ASC file, any errors will be caught by the build function
  auto* file = AscFileManager::read(filename);

  // Note the number of locations so that we can provide some error checking
  auto count = Model::CONFIG->number_of_locations();

  // Iterate through the raster and note the valid locations
  std::vector<int> locations; auto id = 0;
  for (auto row = 0; row < file->NROWS; row++) {
    for (auto col = 0; col < file->NCOLS; col++) {
      if (file->data[row][col] == file->NODATA_VALUE) { continue; }

      if (id > count) {
        throw std::runtime_error(fmt::format("Raster misalignment: pixel count exceeds {} expected while loading {}", count, filename));
      }

      switch (static_cast<int>(file->data[row][col])) {
        case 0: break;
        case 1: locations.emplace_back(id);
          break;
        default:
          throw std::runtime_error(fmt::format("Raster for mutation events should only be zero or one, found {} at {}, {} in {}", file->data[row][col], row, col, filename));
      }

      // Update the id counter
      id++;
    }
  }

  // Verify that the total number of locations matches the location database
  if (id != count) {
    throw std::runtime_error(fmt::format("Raster misalignment: found {} pixels, expected {} while loading {}", id, count, filename));
  }

  // Return the locations
  return locations;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_mutant_event(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event*> events;
    for (const auto & entry : node) {
      // Load the values
      auto start_date = entry["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
      auto district = entry["district"].as<int>();
      auto fraction = entry["fraction"].as<double>();
      auto locus = entry["locus"].as<int>();
      auto mutant_allele = entry["mutant_allele"].as<int>();

      // Make sure the district GIS data is loaded and the district id makes sense
      if (district < 0) {
        LOG(ERROR) << "The target district must be greater than or equal to zero.";
        throw std::invalid_argument("Target district must be greater than or equal to zero");
      }
      if (district > SpatialData::get_instance().get_district_count()) {
        LOG(ERROR) << "Target district is greater than the district count.";
        throw std::invalid_argument("Target district greater than district count.");
      }

      // Make sure the fraction makes sense
      if (fraction <= 0) {
        LOG(ERROR) << "The fraction of the mutants must be greater than zero.";
        throw std::invalid_argument("Mutant fraction must be greater than zero");
      }

      // Make sure the locus and alleles make sense
      if (locus < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Locus must be greater than or equal to zero");
      }
      if (mutant_allele < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Mutant allele must be greater than or equal to zero");
      }

      // Log and add the event to the queue
      auto* event = new IntroduceMutantEvent(time, district, fraction, locus, mutant_allele);
      VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", district: " << district
              << ", locus: " << locus << ", mutant_allele: " << mutant_allele << ", fraction: " << fraction;
      events.push_back(event);
    }
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << IntroduceMutantEvent::EventName << " node: " << error.msg;
    exit(1);
  }
}

std::vector<Event*> PopulationEventBuilder::build_introduce_mutant_raster_event(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event *> events;
    for (const auto &entry: node) {
      // Load the values
      auto start_date = entry["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
      auto filename = entry["raster"].as<std::string>();
      auto fraction = entry["fraction"].as<double>();
      auto locus = entry["locus"].as<int>();
      auto mutant_allele = entry["mutant_allele"].as<int>();

      // Make sure the fraction makes sense
      if (fraction <= 0) {
        LOG(ERROR) << "The fraction of the mutants must be greater than zero.";
        throw std::invalid_argument("Mutant fraction must be greater than zero");
      }

      // Make sure the locus and alleles make sense
      if (locus < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Locus must be greater than or equal to zero");
      }
      if (mutant_allele < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Mutant allele must be greater than or equal to zero");
      }

      // Load the locations from the file, then add the new event to the queue
      auto locations = get_locations_from_raster(filename);
      auto *event = new IntroduceMutantRasterEvent(time, locations, fraction, locus, mutant_allele);
      VLOG(1) << "Adding " << event->name() << " start: " << start_date << " with " << locations.size() << " locations"
              << ", locus: " << locus << ", mutant_allele: " << mutant_allele << ", fraction: " << fraction;
      events.push_back(event);
    }
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << IntroduceMutantRasterEvent::EventName << " node: " << error.msg;
    exit(EXIT_FAILURE );
  } catch (std::runtime_error &error) {
    // Runtime errors are most likely to be thrown when reading the ASC file, so just echo what it says
    LOG(ERROR) << "Unrecoverable error processing YAML file: " << error.what();
    exit(EXIT_FAILURE);
  }
}