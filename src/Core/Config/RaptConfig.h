/*
 * RaptConfig.h
 *
 * Define the RaptConfig class and the RaptConfigEntry struct, these are used for the assessment of the regular
 * administration of prophylactic therapy (RAPT) which requires some deep linkages with the underlying simulation
 * to run correctly.
 */
#pragma once

#include "ConfigItem.hxx"

class Config;

struct RaptConfigEntry {
  bool isDefined = true;
  int period;
  int therapy_id;
  double compliance;
  int age_start;
  int start_day;
};

class RaptConfig : public IConfigItem {
public:
  RaptConfig(const RaptConfig &) = delete;
  RaptConfig &operator=(const RaptConfig &) = delete;

public:
  explicit RaptConfig(const std::string &name, RaptConfigEntry default_value, Config *config = nullptr)
      : IConfigItem(config, name), value_ { default_value } {}

  ~RaptConfig() override = default;

  void set_value(const YAML::Node &node) override;

  virtual RaptConfigEntry &operator()() { return value_; }

private:
  RaptConfigEntry value_;
};
