#ifndef SINGLEROUNDMDAEVENT_H
#define SINGLEROUNDMDAEVENT_H

#include "Events/Event.h"
#include <vector>
#include "Core/PropertyMacro.h"

class SingleRoundMDAEvent : public Event {
 DISALLOW_COPY_AND_ASSIGN(SingleRoundMDAEvent)

 DISALLOW_MOVE(SingleRoundMDAEvent)

 public:
  std::vector<double> fraction_population_targeted;
  int days_to_complete_all_treatments{14};

  explicit SingleRoundMDAEvent(const int &execute_at);

  virtual ~SingleRoundMDAEvent() = default;

  std::string name() override {
    return "SingleRoundMDAEvent";
  }

 private:
  void execute() override;
};

#endif // SINGLEROUNDMDAEVENT_H
