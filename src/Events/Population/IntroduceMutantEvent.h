/**
 * IntroduceMutantEvent.h
 *
 * Introduce mutant genotypes into the simulation by randomly selecting infected individuals and switching the relevant
 * genotype from the wild type to the specified mutant.
 */
#ifndef INTRODUCEMUTANTEVENT_H
#define INTRODUCEMUTANTEVENT_H

#include <string>

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Events/Event.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"

class IntroduceMutantEvent : public Event {
  DISALLOW_COPY_AND_ASSIGN(IntroduceMutantEvent)
  DISALLOW_MOVE(IntroduceMutantEvent)

private:
  int district_;
  double fraction_;
  int locus_;
  int mutant_allele_;

  void execute() override;
  double calculate(std::vector<int> &locations) const;
  int mutate(std::vector<int> &locations, double target_fraction) const;

public:
  inline static const std::string EventName = "introduce_mutant_event";

  explicit IntroduceMutantEvent(const int& time, const int& district, const double& fraction,
                                const int& locus, const int& mutant_allele);
  ~IntroduceMutantEvent() override = default;

  // Return the name of this event
  std::string name() override { return EventName; }
};

#endif
