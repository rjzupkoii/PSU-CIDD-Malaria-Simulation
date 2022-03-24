/*
 * SeasonalRainfall.cpp
 *
 * Implement the seasonal rainfall based seasonal model.
 */
#include "SeasonalInfo.h"

#include "Helpers/TimeHelpers.h"

SeasonalRainfall *SeasonalRainfall::build(const YAML::Node &node) {
  // Prepare the object to be returned
  auto value = new SeasonalRainfall();

  // Note the node for settings
  auto settings = node["rainfall"];

  // Check to make sure the nodes exist
  if (settings["filename"].IsNull()) {
    throw std::invalid_argument("The seasonal rainfall filename parameter is missing.");
  }
  if (settings["period"].IsNull()) {
    throw std::invalid_argument("The seasonal rainfall period parameter is missing.");
  }

  // Read the adjustments, verify that the period matches the adjustment count
  auto filename = settings["filename"].as<std::string>();
  value->read(filename);
  value->period = settings["period"].as<int>();
  if (value->adjustments.size() != value->period) {
    throw std::invalid_argument(fmt::format("The number of rainfall data points ({}) should match the period ({}).", value->adjustments.size(), value->period));
  }

  // Return the object
  return value;
}

double SeasonalRainfall::get_seasonal_factor(const date::sys_days &today, const int &location) {
  // Get the day of the year
  auto doy = TimeHelpers::day_of_year(today);

  // If it's a leap day, return the last day a second time, otherwise just return the value for the day
  return (doy == 366) ? adjustments[doy - 1] : adjustments[doy];
}

void SeasonalRainfall::read(std::string &filename) {
  // Check the validity of the file
  std::ifstream in(filename);
  if (!in.good()) {
    throw std::runtime_error("Error opening the rainfall data file: " + filename);
  }
  if (in.peek() == std::ifstream::traits_type ::eof()) {
    throw std::runtime_error("EOF encountered at start of rainfall data file: " + filename);
  }

  // Read and store the data
  double data;
  while (in >> data) {
    adjustments.emplace_back(data);
  }
}