#include "CustomConfigItem.h"
#include "Config.h"
#include <date/date.h>

void total_time::set_value(const YAML::Node& node) {
  value_ = (date::sys_days{config_->ending_date()} - date::sys_days(config_->starting_date())).count();
}
