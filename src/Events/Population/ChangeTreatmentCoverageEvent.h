#ifndef CHANGETREATMENTCOVERAGEEVENT_H
#define CHANGETREATMENTCOVERAGEEVENT_H

#include "Events/Event.h"
#include "Malaria/ITreatmentCoverageModel.h"

class ChangeTreatmentCoverageEvent : public Event {

 public:
  ITreatmentCoverageModel *treatment_coverage_model;

  explicit ChangeTreatmentCoverageEvent(ITreatmentCoverageModel *tcm);

  virtual ~ChangeTreatmentCoverageEvent();

  std::string name() override {
    return "ChangeTreatmentCoverageEvent";
  }

 private:
  void execute() override;
};

#endif // CHANGETREATMENTCOVERAGEEVENT_H
