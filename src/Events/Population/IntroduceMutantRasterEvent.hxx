/**
 * IntroduceMutantRasterEvent.hxx
 *
 * Introduce mutant genotypes into the simulation by randomly selecting infected individuals and switching the relevant
 * genotype from the wild type to the specified mutant based upon the raster supplied.
 */
#ifndef INTRODUCEMUTANTRASTEREVENT_HXX
#define INTRODUCEMUTANTRASTEREVENT_HXX

#include <utility>

#include "IntroduceMutantEventBase.h"

class IntroduceMutantRasterEvent : public IntroduceMutantEventBase {
  DISALLOW_COPY_AND_ASSIGN(IntroduceMutantRasterEvent)
  DISALLOW_MOVE(IntroduceMutantRasterEvent)

private:
    std::vector<int> locations_;

    void execute() override {
      // Use the locations to calculate the target fraction of mutations and perform them
      auto target_fraction = calculate(locations_);
      auto count = (target_fraction > 0) ? mutate(locations_, target_fraction) : 0;

      // Log the event's operation
      LOG(INFO) << date::year_month_day{scheduler->calendar_date} << " : Introduce mutant by raster event, target fraction: "
                << target_fraction << ", mutations: " << count;
    }

public:
    inline static const std::string EventName = "introduce_mutant_raster_event";

    IntroduceMutantRasterEvent(const int& time, std::vector<int> locations, const double& fraction,
                               const int& locus, const int& mutant_allele) :
      IntroduceMutantEventBase(fraction, locus, mutant_allele), locations_(std::move(locations)) {
      this->time = time;
    }

    ~IntroduceMutantRasterEvent() override = default;

    // Return the name of this event
    std::string name() override { return EventName; }
};

#endif
