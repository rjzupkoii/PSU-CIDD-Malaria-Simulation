#define _USE_MATH_DEFINES

#include "DependentConfigItem.h"
#include "Config.h"
#include <date/date.h>
#include <cmath>
#include <utility>

void total_time::set_value(const YAML::Node& node) {
  value_ = (date::sys_days{config_->ending_date()} - date::sys_days(config_->starting_date())).count();
}

void number_of_age_classes::set_value(const YAML::Node& node) {
  value_ = config_->age_structure().size();
}

void number_of_locations::set_value(const YAML::Node& node) {
  value_ = config_->location_db().size();
}

void spatial_distance_matrix::set_value(const YAML::Node& node) {
  value_.resize(config_->number_of_locations());
  for (auto from_location = 0; from_location < config_->number_of_locations(); from_location++) {
    value_[from_location].resize(static_cast<unsigned long long int>(config_->number_of_locations()));
    for (auto to_location = 0; to_location < config_->number_of_locations(); to_location++) {
      value_[from_location][to_location] = Spatial::Coordinate::calculate_distance_in_km(
        *config_->location_db()[from_location].coordinate,
        *config_->location_db()[to_location].coordinate);
      //
      //            std::cout << "distance[" << from_location << "," << to_location << "]: "
      //                      << spatial_distance_matrix_[from_location][to_location] << std::endl;
    }
  }

}

seasonal_info::seasonal_info(std::string name, SeasonalInfo default_value, Config* config): IConfigItem(config, name),
                                                                                            value_{std::move(default_value)} {
  config_->config_items.emplace_back(this);
}

SeasonalInfo& seasonal_info::operator()() {
	return value_;
}

void seasonal_info::set_value(const YAML::Node& node) {
	auto seasonal_info_node = node[name_];
	value_.A.clear();
	value_.B.clear();
	value_.C.clear();
	value_.phi.clear();
	value_.min_value.clear();
	value_.enable = seasonal_info_node["enable"].as<bool>();

	for (auto i = 0; i < config_->number_of_locations(); i++) {
		auto input_loc = seasonal_info_node["a"].size() < config_->number_of_locations() ? 0 : i;
		value_.A.push_back(seasonal_info_node["a"][input_loc].as<double>());

		const auto period = seasonal_info_node["period"].as<double>();
		auto B = 2 * M_PI / period;

		value_.B.push_back(B);

		const auto phi = seasonal_info_node["phi"][input_loc].as<float>();
		value_.phi.push_back(phi);
		auto C = -phi * B;
		value_.C.push_back(C);

		value_.min_value.push_back(seasonal_info_node["min_value"][input_loc].as<float>());
	}
}

