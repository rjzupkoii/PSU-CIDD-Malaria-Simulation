/* 
 * File:   InfantImmuneComponent.h
 * Author: nguyentran
 *
 * Created on May 28, 2013, 11:06 AM
 */

#ifndef INFANTIMMUNECOMPONENT_H
#define    INFANTIMMUNECOMPONENT_H

#include "ImmuneComponent.h"
#include "ObjectPool.h"

class InfantImmuneComponent : public ImmuneComponent {
//OBJECTPOOL(InfantImmuneComponent)

DISALLOW_COPY_AND_ASSIGN_(InfantImmuneComponent)

public:
    InfantImmuneComponent(ImmuneSystem *immune_system = nullptr);

    // InfantImmuneComponent(const InfantImmuneComponent& orig);
    virtual ~InfantImmuneComponent();

    virtual double get_decay_rate(const int &age = 0) const;

    virtual double get_acquire_rate(const int &age = 0) const;

    virtual double get_current_value();

private:

};

#endif    /* INFANTIMMUNECOMPONENT_H */

