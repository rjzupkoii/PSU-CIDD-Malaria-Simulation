/*
 * UpdateEcozoneEvent.hxx
 * 
 * Update the ecozone from the previous type to the new type.
 */
#ifndef UPDATEECOZONEEVENT_H
#define UPDATEECOZONEEVENT_H

#include "Core/Config/Config.h"
#include "Core/TypeDef.h"
#include "Environment/SeasonalInfo.h"
#include "Events/Event.h"
#include "Model.h"

class UpdateEcozoneEvent : public Event {
  private:
    int from_;
    int to_;

    // Execute the import event
    void execute() override {
      
      // Scan all the locations, if they match the old values, then replace them with the new
      LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " Updating ecozone " << from_ << " to " << to_;
      
      // Defer to the object for the actual update
      auto seasons = dynamic_cast<SeasonalEquation*>(Model::CONFIG->seasonal_info());
      if (seasons == nullptr) {
        throw std::runtime_error("Configuration called for seasonality to be updated with a mode that does not support it.");
      }
      seasons->update_seasonality(from_, to_);
    }

  public:
    inline static const std::string EventName = "update_ecozone_event";

    UpdateEcozoneEvent(int from, int to, int start) : from_(from), to_(to) {
      time = start;
    }

    ~UpdateEcozoneEvent() override = default;

    std::string name() override { return EventName; }

};

#endif