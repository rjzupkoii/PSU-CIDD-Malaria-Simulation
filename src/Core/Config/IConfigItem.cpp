#include "IConfigItem.h"
#include "Config.h"

IConfigItem::IConfigItem(Config* config, std::string name) : config_{config}, name_{std::move(name)} {
  config_->config_items.emplace_back(this);
}
