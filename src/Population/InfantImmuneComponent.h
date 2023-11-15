/* 
 * InfantImmuneComponent.h
 *
 * Define the immune component for individuals that are infants.
 */
#ifndef INFANTIMMUNECOMPONENT_H
#define INFANTIMMUNECOMPONENT_H

#include "ImmuneComponent.h"

class InfantImmuneComponent : public ImmuneComponent {

 DISALLOW_COPY_AND_ASSIGN(InfantImmuneComponent)

 public:
  explicit InfantImmuneComponent(ImmuneSystem *immune_system = nullptr);

  ~InfantImmuneComponent() override = default;

  [[nodiscard]] double get_decay_rate(const int &age) const override;

  [[nodiscard]] double get_acquire_rate(const int &age) const override;

  double get_current_value() override;

};

#endif
