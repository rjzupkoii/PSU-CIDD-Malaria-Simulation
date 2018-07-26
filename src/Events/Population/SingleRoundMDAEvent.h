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
  std::vector<double> fraction_of_targeted_population_participating;
  int age_class_min{0};
  int age_class_max{100};

  explicit SingleRoundMDAEvent(const int& execute_at);

  virtual ~SingleRoundMDAEvent() = default;

  std::string name() override {
    return "SingleRoundMDAEvent";
  }

private:
  void execute() override;
};
#endif // SINGLEROUNDMDAEVENT_H
