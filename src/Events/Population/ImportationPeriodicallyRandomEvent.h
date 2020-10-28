/*
 * ImportationPeriodicallyRandomEvent.hxx
 * 
 * Import the indicated genotype on a populated weighted basis at the time 
 * step indicated, reschedule the event to occur periodically afterwards.
 */
#ifndef IMPORTATIONPERIODICALLYRANDOMEVENT_HXX
#define IMPORTATIONPERIODICALLYRANDOMEVENT_HXX

#include "easylogging++.h"

#include "Events/Event.h"
#include "Population/Person.h"

class ImportationPeriodicallyRandomEvent : public Event {
  private:
    int genotypeId_ = 0;
    int periodicity_ = 0;

    // Execute the import event
    void execute() override;

    // Get a random index to perform the importation event at, the population 
    // is used to weight the random pull
    int get_location();

    // Inflict the act infection upon the individual
    void infect(Person* person, int genotypeId);

  public:
    inline static const std::string EventName = "importation_periodically_random_event";

    ImportationPeriodicallyRandomEvent(int genotypeId, int start, int periodicity) 
      : genotypeId_(genotypeId), periodicity_(periodicity) { time = start; }
    ~ImportationPeriodicallyRandomEvent() = default;

    // Return the name of this event
    std::string name() override { return EventName; }

};

#endif