/* 
 * File:   Dispatcher.h
 * Author: nguyentran
 *
 * Created on May 3, 2013, 3:46 PM
 */

#ifndef DISPATCHER_H
#define	DISPATCHER_H

#include "PropertyMacro.h"
#include "TypeDef.h"

class Event;

class Dispatcher {
    DISALLOW_COPY_AND_ASSIGN_(Dispatcher)
    POINTER_PROPERTY(EventPtrVector, events);


public:
    Dispatcher();
    //    Dispatcher(const Dispatcher& orig);
    virtual ~Dispatcher();

    void init();

    virtual void add(Event* event);
    virtual void remove(Event* event);
    virtual void clear_events();
    virtual void update();

private:

};

#endif	/* DISPATCHER_H */

