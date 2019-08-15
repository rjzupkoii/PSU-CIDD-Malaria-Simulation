/* 
 * File:   ImmuneComponent.h
 * Author: nguyentran
 *
 * Created on May 27, 2013, 12:44 PM
 */

#ifndef IMMUNECOMPONENT_H
#define    IMMUNECOMPONENT_H

#include "Core/PropertyMacro.h"

//#include "ObjectPool.h"
class ImmuneSystem;

class Model;

class ImmuneComponent {
  //    //OBJECTPOOL(ImmuneComponent)
 DISALLOW_COPY_AND_ASSIGN(ImmuneComponent)

 POINTER_PROPERTY(ImmuneSystem, immune_system)

 PROPERTY_REF(double, latest_value)

 public:
  explicit ImmuneComponent(ImmuneSystem *immune_system = nullptr);

  //    ImmuneComponent(const ImmuneComponent& orig);
  virtual ~ImmuneComponent();

  void update();

  virtual void draw_random_immune();

  virtual double get_current_value();

  virtual double get_decay_rate(const int &age = 0) const = 0;

  virtual double get_acquire_rate(const int &age = 0) const = 0;

};

#endif    /* IMMUNECOMPONENT_H */
