/*
 * EnvironmentEventBuilder.cpp
 * 
 * Implement the environmental event builders.
 */
#include "EnvironmentEventBuilder.h"

#include <vector>

#include "Core/Config/Config.h"
#include "UpdateEcozoneEvent.hxx"
#include "yaml-cpp/yaml.h"

std::vector<Event*> EnvironmentEventBuilder::build(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  const auto name = node["name"].as<std::string>();

  if (name == UpdateEcozoneEvent::EventName) {
    events = build_update_ecozone_event(node["info"], config);
  }

  return events;
}

std::vector<Event*> EnvironmentEventBuilder::build_update_ecozone_event(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event*> events;
    for (std::size_t ndx = 0; ndx < node.size(); ndx++) {
      // Load the values
      auto start_date = node[ndx]["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
      auto from = node[ndx]["from"].as<int>();
      auto to = node[ndx]["to"].as<int>();
        
      // Verify inputs
      if (from < 0) {
        LOG(ERROR) << "Invalid from value supplied for " << UpdateEcozoneEvent::EventName
                   << " value supplied cannot be less than zone";
        throw std::invalid_argument("From value cannot be less than zero");
      }
      if (to < 0) {
        LOG(ERROR) << "Invalid to value supplied for " << UpdateEcozoneEvent::EventName
                   << " value supplied cannot be less than zero";
        throw std::invalid_argument("To value cannot be less than zero");
      }

      // Log and add the event to the queue
      auto* event = new UpdateEcozoneEvent(from, to, time);
      VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", from: " << from << ", to: " << to;
      events.push_back(event);
    }
    return events;

  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << UpdateEcozoneEvent::EventName << " node: " << error.msg;
    exit(1);        
  }
}