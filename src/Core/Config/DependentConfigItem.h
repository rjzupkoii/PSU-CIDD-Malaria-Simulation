#ifndef CUSTOMCONFIGITEM_H
#define CUSTOMCONFIGITEM_H

#include <string>
#include "ConfigItem.h"
#include "DrugDatabase.h"

namespace YAML {
  class Node;
}

class Config;

class total_time : public ConfigItem<int> {
public:
  total_time(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};

class number_of_age_classes : public ConfigItem<int> {
public:
  number_of_age_classes(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};

class number_of_locations : public ConfigItem<int> {
public:
  number_of_locations(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}
  void set_value(const YAML::Node& node) override;
};

class spatial_distance_matrix : public ConfigItem<std::vector<std::vector<double>>> {
public:
  spatial_distance_matrix(std::string name, const std::vector<std::vector<double>>& default_value, Config* config) : ConfigItem<std::vector<
    std::vector<double>>>(
    name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};


class seasonal_info : public IConfigItem {
protected:
	SeasonalInfo value_{};
public:
	//constructor
  explicit seasonal_info(std::string name, SeasonalInfo default_value, Config* config = nullptr);
	// destructor
	virtual ~seasonal_info() = default;
	// copy constructor
	seasonal_info(const seasonal_info&) = delete;
	// copy assignment
	seasonal_info& operator=(const seasonal_info&) = delete;
	// move constructor
	seasonal_info(seasonal_info&&) = delete;
	// move assignment
	seasonal_info& operator=(seasonal_info&&) = delete;

	virtual SeasonalInfo& operator()();

	void set_value(const YAML::Node& node) override;
};

#endif // CUSTOMCONFIGITEM_H
