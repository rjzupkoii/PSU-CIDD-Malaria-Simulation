/* 
 * NonInfantImmuneComponent.h
 *
 * Define the immune component for individuals that are not infants.
 */
#ifndef NONINFANTIMMUNECOMPONENT
#define NONINFANTIMMUNECOMPONENT

#include "Core/ObjectPool.h"
#include "ImmuneComponent.h"

class NonInfantImmuneComponent : public ImmuneComponent {
 DISALLOW_COPY_AND_ASSIGN(NonInfantImmuneComponent)

 public:
  explicit NonInfantImmuneComponent(ImmuneSystem *immune_system = nullptr);

  ~NonInfantImmuneComponent() override = default;

  [[nodiscard]] double get_decay_rate(const int &age) const override;

  [[nodiscard]] double get_acquire_rate(const int &age) const override;

};

#endif

