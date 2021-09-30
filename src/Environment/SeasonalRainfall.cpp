/*
 * SeasonalRainfall.cpp
 *
 * Implement the seasonal rainfall based seasonal model.
 */
#include "SeasonalInfo.h"

SeasonalRainfall *SeasonalRainfall::build(const YAML::Node &node, Config *config) {
  return nullptr;
}

double SeasonalRainfall::get_seasonal_factor(const date::sys_days &today, const int &location) {
  return ISeasonalInfo::get_seasonal_factor(today, location);
}