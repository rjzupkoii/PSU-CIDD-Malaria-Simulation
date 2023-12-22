/*
 * ChangeCirculationPercentEvent.hxx
 *
 * Define and implement an event to change the daily percentage of the population circulating.
 */
#ifndef CHANGE_CIRCULATION_PERCENT_EVENT_HXX
#define CHANGE_CIRCULATION_PERCENT_EVENT_HXX

#include "Core/Config/Config.h"
#include "Events/Event.h"
#include "Model.h"

class ChangeCirculationPercentEvent : public Event {
private:
    float rate_ = 0.0;

    // Execute the event to change the circulation percentage
    void execute() override {
      Model::CONFIG->circulation_info().circulation_percent = rate_;

      // Log on demand
      VLOG(1) << "Updated circulation_percent to " << rate_;
    }

public:
    inline static const std::string EventName = "change_circulation_percent_event";

    ChangeCirculationPercentEvent(float rate, int start) : rate_(rate) { time = start; }
    ~ChangeCirculationPercentEvent() override = default;

    // Return the name of this event
    std::string name() override { return EventName; }

};

#endif