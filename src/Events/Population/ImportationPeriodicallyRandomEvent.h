/*
 * ImportationPeriodicallyRandomEvent.h
 * 
 * Import the indicated genotype on a populated weighted basis at the time 
 * step indicated, reschedule the event to occur periodically afterwards.
 */
#ifndef IMPORTATIONPERIODICALLYRANDOMEVENT_HXX
#define IMPORTATIONPERIODICALLYRANDOMEVENT_HXX

#include "easylogging++.h"
#include <date/date.h>

#include "Events/Event.h"
#include "Population/Person.h"

class ImportationPeriodicallyRandomEvent : public Event {
  private:
    int count_ = 0;
    int genotypeId_ = 0;
    double log_parasite_density_ = 0.0;

    // Execute the import event
    void execute() override;

    // Get a random index to perform the importation event at, the population 
    // is used to weight the random pull
    static std::size_t get_location();

    // Inflict the act infection upon the individual
    void infect(Person* person, int genotypeId) const;

  public:
    inline static const std::string EventName = "importation_periodically_random_event";

    ImportationPeriodicallyRandomEvent(int genotypeId, int start, int count, double log_parasite_density)
      : genotypeId_(genotypeId), count_(count)
    { 
      time = start; 
      log_parasite_density_ = log_parasite_density;
    }
    ~ImportationPeriodicallyRandomEvent() override = default;

    // Return the name of this event
    std::string name() override { return EventName; }

};

#endif