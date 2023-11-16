/* 
 * ImmuneSystem.h
 *
 * Define the immune system for the individuals.
 */
#ifndef IMMUNE_SYSTEM_H
#define IMMUNE_SYSTEM_H

#include "Core/ObjectPool.h"
#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class Config;
class ImmuneComponent;
class Model;
class Person;

class ImmuneSystem {
 OBJECTPOOL(ImmuneSystem)

 DISALLOW_COPY_AND_ASSIGN(ImmuneSystem)

 POINTER_PROPERTY(Person, person)

 VIRTUAL_PROPERTY_REF(bool, increase)

 POINTER_PROPERTY_HEADER(ImmuneComponent, immune_component)

 public:
  explicit ImmuneSystem(Person *p = nullptr);

  virtual ~ImmuneSystem();

  virtual void update();

  [[nodiscard]] virtual double get_latest_immune_value() const;

  virtual void set_latest_immune_value(double value);

  [[nodiscard]] virtual double get_current_value() const;

  [[nodiscard]] virtual double
  get_parasite_size_after_t_days(const int &duration, const double &originalSize, const double &fitness) const;

  [[nodiscard]] virtual double get_clinical_progression_probability() const;

};

#endif
