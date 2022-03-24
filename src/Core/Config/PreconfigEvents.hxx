/*
 * PreconfigEvents.hxx
 * 
 * Parse the preconfigured events from the YAML file and allow the relevent 
 * builder to add them to the schedule.
 */
#ifndef PRECONFIGEVENTS_HXX
#define PRECONFIGEVENTS_HXX

#include "ConfigItem.hxx"
#include "Events/Environment/EnvironmentEventBuilder.h"
#include "Events/Population/PopulationEventBuilder.h"

class PreconfigEvents : public IConfigItem {
  DISALLOW_COPY_AND_ASSIGN(PreconfigEvents)
  DISALLOW_MOVE(PreconfigEvents)

  public:
    std::vector<Event *> value_;

  public:
    explicit PreconfigEvents(const std::string &name, std::vector<Event *> default_value, Config *config = nullptr) 
      : IConfigItem(config, name), value_{std::move(default_value)} { }

    virtual ~PreconfigEvents() = default;

    virtual std::vector<Event *> &operator()() { return value_; }

    void set_value(const YAML::Node &node) override {
      for (std::size_t i = 0; i < node["events"].size(); ++i) {
        // Attempt to add a population event
        auto events = PopulationEventBuilder::build(node["events"][i], config_);
        if (events.size() != 0) {
          value_.insert(value_.end(), events.begin(), events.end());
          continue;
        }

        // Attempt to add an enviornment event
        events = EnvironmentEventBuilder::build(node["events"][i], config_);
        if (events.size() != 0) {
          value_.insert(value_.end(), events.begin(), events.end());
        }
      }
    }
};

#endif