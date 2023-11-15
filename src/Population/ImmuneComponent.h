/* 
 * ImmuneComponent.h
 *
 * Define the immune component class, this class is responsible for
 */
#ifndef IMMUNECOMPONENT_H
#define IMMUNECOMPONENT_H

#include "Core/PropertyMacro.h"

class ImmuneSystem;
class Model;

class ImmuneComponent {

 DISALLOW_COPY_AND_ASSIGN(ImmuneComponent)

 POINTER_PROPERTY(ImmuneSystem, immune_system)

 PROPERTY_REF(double, latest_value)

 public:
  explicit ImmuneComponent(ImmuneSystem *immune_system = nullptr);

  virtual ~ImmuneComponent();

  void update();

  virtual void draw_random_immune();

  virtual double get_current_value();

  [[nodiscard]] virtual double get_decay_rate(const int &age) const = 0;

  [[nodiscard]] virtual double get_acquire_rate(const int &age) const = 0;

};

#endif
