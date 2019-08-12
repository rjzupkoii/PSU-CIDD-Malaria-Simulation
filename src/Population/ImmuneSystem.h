/* 
 * File:   ImmuneSystem.h
 * Author: nguyentran
 *
 * Created on May 27, 2013, 11:56 AM
 */

#ifndef IMMUNESYSTEM_H
#define    IMMUNESYSTEM_H

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"
#include "Core/ObjectPool.h"

class Model;

class Person;

class ImmuneComponent;

class Config;

//typedef std::vector<ImmuneComponent*> ImmuneComponentPtrVector;

class ImmuneSystem {
 OBJECTPOOL(ImmuneSystem)

 DISALLOW_COPY_AND_ASSIGN(ImmuneSystem)

 POINTER_PROPERTY(Person, person)

 VIRTUAL_PROPERTY_REF(bool, increase)
  //    POINTER_PROPERTY_REF(ImmuneComponentPtrVector, immune_components);
 POINTER_PROPERTY_HEADER(ImmuneComponent, immune_component)

 public:
  explicit ImmuneSystem(Person *p = nullptr);

  virtual ~ImmuneSystem();

  //    virtual void clear();

  virtual void draw_random_immune();

  virtual void update();

  virtual double get_lastest_immune_value() const;

  virtual void set_latest_immune_value(double value);

  virtual double get_current_value() const;

  virtual double
  get_parasite_size_after_t_days(const int &duration, const double &originalSize, const double &fitness) const;

  virtual double get_clinical_progression_probability() const;

};

#endif    /* IMMUNESYSTEM_H */
