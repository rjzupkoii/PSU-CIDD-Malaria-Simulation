#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include "IConfigItem.h"
#include "Core/YAML/YamlConverter.h"
#include <yaml-cpp/yaml.h>
#include "easylogging++.h"
#include <Helpers/StringHelpers.h>

#define CONFIG_ITEM(name,type,default_value)\
  ConfigItem<type> name{#name,default_value, this};
#define CUSTOM_CONFIG_ITEM(name,default_value)\
  name name{#name,default_value, this};


class Config;

template <typename T>
class ConfigItem : public IConfigItem {
protected:
  Config* config_;
  std::string name_;
  T value_{};
public:
  //constructor
  explicit ConfigItem(std::string name, const T& default_value, Config* config = nullptr) : config_{config},
                                                                                            name_{std::move(name)},
                                                                                            value_{
                                                                                              std::move(default_value)
                                                                                            } {
    // ReSharper disable once CppClassIsIncomplete
    config_->config_items.emplace_back(this);
  }

  // destructor
  virtual ~ConfigItem() = default;
  // copy constructor
  ConfigItem(const ConfigItem&) = delete;

  // copy assignment
  ConfigItem& operator=(const ConfigItem&) = delete;

  // move constructor
  ConfigItem(ConfigItem&&) = delete;

  // move assignment
  ConfigItem& operator=(ConfigItem&&) = delete;

  virtual T& operator()() {
    return value_;
  };

  void set_value(const YAML::Node& node) override {
    if (node[name_]) {
      value_ = node[name_].as<T>();
    }
    else {
      // LOG(WARNING) << name_ << " used default value of " << value_ << std::endl;
      LOG(WARNING) << name_ << " used default value." << std::endl;
    }
  }

  const std::string& name() override {
    return name_;
  }
};


#endif // CONFIGITEM_H
