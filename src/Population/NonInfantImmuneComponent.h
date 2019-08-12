/* 
 * File:   NormalImmuneComponent.h
 * Author: nguyentran
 *
 * Created on May 28, 2013, 11:06 AM
 */

#ifndef NONINFANTIMMUNECOMPONENT
#define    NONINFANTIMMUNECOMPONENT

#include "ImmuneComponent.h"
#include "Core/ObjectPool.h"

class NonInfantImmuneComponent : public ImmuneComponent {
 DISALLOW_COPY_AND_ASSIGN(NonInfantImmuneComponent)
//    OBJECTPOOL(NonInfantImmuneComponent)
 public:
  NonInfantImmuneComponent(ImmuneSystem *immune_system = nullptr);

  // NonInfantImmuneComponent(const NonInfantImmuneComponent& orig);
  virtual ~NonInfantImmuneComponent();

  virtual double get_decay_rate(const int &age = 0) const;

  virtual double get_acquire_rate(const int &age = 0) const;

 private:

};

#endif    /* NONINFANTIMMUNECOMPONENT */

