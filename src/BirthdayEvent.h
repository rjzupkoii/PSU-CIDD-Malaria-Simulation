/* 
 * File:   BirthdayEvent.h
 * Author: nguyentran
 *
 * Created on May 9, 2013, 2:42 PM
 */

#ifndef BIRTHDAYEVENT_H
#define	BIRTHDAYEVENT_H
#include "Event.h"
#include "ObjectPool.h"

class Person;

class BirthdayEvent : public Event {
    OBJECTPOOL(BirthdayEvent)
    DISALLOW_COPY_AND_ASSIGN_(BirthdayEvent)
public:
    BirthdayEvent();
//    BirthdayEvent(const BirthdayEvent& orig);
    virtual ~BirthdayEvent();

    static void schedule_event(Scheduler* scheduler, Person* p, const int& time);
    
    virtual std::string name(){
        return "Birthday Event";
    }
private:
    virtual void execute();
};

#endif	/* BIRTHDAYEVENT_H */

