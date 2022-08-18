/*
 * RaptConfig.cpp
 *
 * Implement the RaptConfig class.
 */
#include "rapt_config.h"

#include "Core/Config/Config.h"

void rapt_config::set_value(const YAML::Node &node) {
  if (node[name_]) {
    value_.isDefined = true;
    auto rc_node = node[name_];
    value_.period = rc_node["period"].as<int>();
    value_.therapy_id = rc_node["therapy_id"].as<int>();
    value_.compliance = rc_node["compliance"].as<double>();
    value_.age_start = rc_node["age_start"].as<int>();

    const auto ymd = rc_node["start_day"].as<date::year_month_day>();
    value_.start_day = (date::sys_days { ymd } - date::sys_days(config_->starting_date())).count();
  } else {
    value_.isDefined = false;
  }
}
