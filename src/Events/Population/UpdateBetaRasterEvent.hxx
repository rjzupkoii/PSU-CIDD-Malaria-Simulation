/*
 * UpdateBetaRasterEvent.hxx
 *
 * Update all beta values based upon the raster file indicated.
 */

#ifndef UPDATEBETARASTEREVENT_H
#define UPDATEBETARASTEREVENT_H

#include <utility>

#include "Core/Config/Config.h"
#include "Events/Event.h"
#include "GIS/AscFile.h"
#include "Model.h"

class UpdateBetaRasterEvent : public Event {
  private:
    std::string filename_;

    // Execute the event to replace all beta values
    void execute() override {
      // Read the raster file
      AscFile* raster = AscFileManager::read(filename_);

      // Grab a reference to the location_db to work with, note the location count
      auto& location_db = Model::CONFIG->location_db();
      auto count = Model::CONFIG->number_of_locations();

      // Iterate though and update the betas
      auto id = 0;
      for (auto row = 0; row < raster->NROWS; row++) {
        for (auto col = 0; col < raster->NCOLS; col++) {
          // Check for no data, press on if so
          if (raster->data[row][col] == raster->NODATA_VALUE) { continue; }

          // Check for raster misalignment
          if (id > count) {
            throw std::runtime_error("Raster misalignment updating betas, pixel count exceeds number of cells in model.");
          }

          // Update the beta at the location
          location_db[id].beta = raster->data[row][col];
          id++;
        }
      }

      // Work complete, log it as info
      LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " Updated betas based on " << filename_;
    }

  public:
    inline static const std::string EventName = "update_beta_raster_event";

    UpdateBetaRasterEvent(std::string filename, int start) : filename_(std::move(filename)) {
      time = start;
    }

    ~UpdateBetaRasterEvent() override = default;

    std::string name() override { return EventName; }
};

#endif
