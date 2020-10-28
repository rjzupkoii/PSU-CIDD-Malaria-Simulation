/*
 * AnnualBetaUpdateEvent.hxx
 * 
 * This event class provides the ability to alter the beta parameter on an 
 * annual basis.
 */
#ifndef ANNUALBETAUPDATEEVENT_HXX
#define ANNUALBETAUPDATEEVENT_HXX

#include <math.h>

#include "easylogging++.h"

#include "Core/Config/Config.h"
#include "Core/Scheduler.h"
#include "Events/Event.h"
#include "Helpers/TimeHelpers.h"
#include "Model.h"

class AnnualBetaUpdateEvent : public Event {
  private:
    float rate_ = 0.0;

    // Execute the annual beta update event
    void execute() override {

        // Grab a reference to the location_db to work with
        auto& location_db = Model::CONFIG->location_db();

        // Iterate through and adjust the betas
        auto count = Model::CONFIG->number_of_locations();
        for (auto ndx = 0; ndx < count; ndx++) {
            location_db[ndx].beta = adjust(location_db[ndx].beta, rate_);
        }

        // Schedule for one year from now
        auto time = scheduler->current_time() + TimeHelpers::number_of_days_to_next_year(scheduler->calendar_date);
        AnnualBetaUpdateEvent* event = new AnnualBetaUpdateEvent(rate_, time);
        scheduler->schedule_population_event(event);

        // Log on demand
        VLOG(1) << "Updated beta by " << rate_ << ", scheduled next for: " << time;
    }

    // Update the bete by the given rate, round up ate the fifth decimal place but clamp at 0.0
    double adjust(double beta, double rate) {
        beta += (beta * rate);
        beta = int(beta * pow(10, 5)) / pow(10, 5);
        return (beta < 0.0) ? 0.0 : beta;
    }

  public:  
    inline static const std::string EventName = "annual_beta_update_event";

    AnnualBetaUpdateEvent(float rate, int start) : rate_(rate) { time = start; }
    ~AnnualBetaUpdateEvent() = default;

    // Return the name of this event
    std::string name() override { return EventName; }

};

#endif