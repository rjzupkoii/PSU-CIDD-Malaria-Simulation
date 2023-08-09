/**
 * IntroduceMutantEventBase.h
 *
 * Base class for introduce mutant event.
 */
#ifndef INTRODUCEMUTANTEVENTBASE_H
#define INTRODUCEMUTANTEVENTBASE_H

#include <vector>

#include "Core/PropertyMacro.h"
#include "Events/Event.h"

class IntroduceMutantEventBase : public Event {

  protected:
    double fraction_;
    int locus_;
    int mutant_allele_;

    IntroduceMutantEventBase(const double& fraction, const int& locus, const int& mutant_allel) :
      fraction_(fraction), locus_(locus), mutant_allele_(mutant_allel) { }

    double calculate(std::vector<int> &locations) const;
    int mutate(std::vector<int> &locations, double target_fraction) const;
};

#endif
