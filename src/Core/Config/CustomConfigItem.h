
#ifndef CUSTOMCONFIGITEM_H
#define CUSTOMCONFIGITEM_H

#include <string>
#include "ConfigItem.h"

namespace YAML {
  class Node;
}
class Config;

class total_time : public ConfigItem<int> {
public:
	total_time(const std::string& name, const int& default_value, Config* config)
		: ConfigItem<int>(name, default_value, config) { }

	void set_value(const YAML::Node& node) override;
};
#endif // CUSTOMCONFIGITEM_H
