#pragma once

#include "ConfigItem.h"

class Config;

struct RaptConfig {
  bool isDefined = true;
  int period;
  int therapy_id;
  double compliance;
  int age_start;
  int start_day;
};

class rapt_config : public IConfigItem {
public:
  rapt_config(const rapt_config &) = delete;
  rapt_config &operator=(const rapt_config &) = delete;

public:
  explicit rapt_config(const std::string &name, RaptConfig default_value,
                       Config *config = nullptr)
      : IConfigItem(config, name), value_ { default_value } {}

  ~rapt_config() override = default;

  void set_value(const YAML::Node &node) override;

  virtual RaptConfig &operator()() { return value_; }

private:
  RaptConfig value_;
};
