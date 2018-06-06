/* 
 * File:   Event.h
 * Author: nguyentran
 *
 * Created on May 3, 2013, 3:13 PM
 */

#ifndef EVENT_H
#define	EVENT_H

#include "IndexHandler.h"
#include "PropertyMacro.h"
#include <string>
class Dispatcher;
class Scheduler;

class Event : public IndexHandler {
    DISALLOW_COPY_AND_ASSIGN_(Event)

    POINTER_PROPERTY(Scheduler, scheduler);
    POINTER_PROPERTY(Dispatcher, dispatcher);
    PROPERTY_REF(bool, executable);
    PROPERTY_REF(int, time);

public:
    Event();
    //    Event(const Event& orig);
    virtual ~Event();

    void perform_execute();
    virtual std::string name() = 0;
private:
    virtual void execute() = 0;

};

#endif	/* EVENT_H */

