/* 
 * File:   InfantImmuneComponent.h
 * Author: nguyentran
 *
 * Created on May 28, 2013, 11:06 AM
 */

#ifndef INFANTIMMUNECOMPONENT_H
#define    INFANTIMMUNECOMPONENT_H

#include "ImmuneComponent.h"

class InfantImmuneComponent : public ImmuneComponent {
  //OBJECTPOOL(InfantImmuneComponent)

 DISALLOW_COPY_AND_ASSIGN(InfantImmuneComponent)

 public:
  explicit InfantImmuneComponent(ImmuneSystem *immune_system = nullptr);

  // InfantImmuneComponent(const InfantImmuneComponent& orig);
  virtual ~InfantImmuneComponent();

  double get_decay_rate(const int &age) const override;

  double get_acquire_rate(const int &age) const override;

  double get_current_value() override;

};

#endif    /* INFANTIMMUNECOMPONENT_H */
