/* 
 * File:   Event.cpp
 * Author: nguyentran
 * 
 * Created on May 3, 2013, 3:13 PM
 */

#include "Event.h"
#include "Scheduler.h"
#include  "Dispatcher.h"
#include <typeinfo>

Event::Event() : dispatcher_(NULL), scheduler_(NULL), time_(-1), executable_(true), IndexHandler() {
}

Event::~Event() {
    if (dispatcher_ != NULL) {      
        dispatcher_->remove(this);
    }
    dispatcher_ = NULL;
    scheduler_ = NULL;
}

void Event::perform_execute() {
    if (executable()) {
        //        std::cout << "Before event"<< std::endl;     

        //update to current status of the dispatcher
        if (dispatcher_ != NULL) {
            //            std::cout << name() << std::endl;
            dispatcher_->update();
        }

        //std::cout << "1.run" << name()<< std::endl;
        execute();
        //std::cout << "2.e-run" << std::endl;
        if (dispatcher_ != NULL) {
            dispatcher_->remove(this);
            set_dispatcher(NULL);
        }

        set_executable(false);
        //        std::cout << "After event"<< std::endl;     
    }
}