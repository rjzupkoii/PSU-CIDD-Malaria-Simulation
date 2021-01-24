/*
 * UpdateEcozoneEvent.h
 * 
 * Update the ecozone from the previous type to the new type.
 */
#ifndef UPDATEECOZONEEVENT_H
#define UPDATEECOZONEEVENT_H

#include "Core/Config/Config.h"
#include "Core/TypeDef.h"
#include "Events/Event.h"
#include "Model.h"

class UpdateEcozoneEvent : public Event {
  private:
    int from_;
    int to_;

    // Execute the import event
    void execute() override {
      
      // Scan all of the locations, if they match the old values, then replace them with the new
      LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " Updating ecozone " << from_ << " to " << to_;
      
      // Get a pointer to the seasonal information used by the model
      auto& seasons = Model::CONFIG->seasonal_info();

      for (std::size_t ndx = 0; ndx < seasons.base.size(); ndx++) {
        if (seasons.base[ndx] == seasons.reference_base[from_] && 
            seasons.A[ndx] == seasons.reference_A[from_] &&
            seasons.B[ndx] == seasons.reference_B[from_] && 
            seasons.phi[ndx] == seasons.reference_phi[from_]) {
          seasons.base[ndx] = seasons.reference_base[to_];
          seasons.A[ndx] = seasons.reference_A[to_];
          seasons.B[ndx] = seasons.reference_B[to_];
          seasons.phi[ndx] = seasons.reference_phi[to_];
        }
      }
    }

  public:
    inline static const std::string EventName = "update_ecozone_event";

    UpdateEcozoneEvent(int from, int to, int start) : from_(from), to_(to) {
      time = start;
    }

    ~UpdateEcozoneEvent() = default;

    std::string name() override { return EventName; }

};

#endif