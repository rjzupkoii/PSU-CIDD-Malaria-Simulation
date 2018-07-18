/* 
 * File:   Event.cpp
 * Author: nguyentran
 * 
 * Created on May 3, 2013, 3:13 PM
 */

#include "Event.h"
#include  "Dispatcher.h"

Event::Event() :
  scheduler_(nullptr), dispatcher_(nullptr), executable_(true), time_(-1) {}

Event::~Event() {
  if (dispatcher_ != nullptr) {
    dispatcher_->remove(this);
  }
  dispatcher_ = nullptr;
  scheduler_ = nullptr;
}

void Event::perform_execute() {
  if (executable()) {
    //        std::cout << "Before event"<< std::endl;     

    //update to current status of the dispatcher
    if (dispatcher_ != nullptr) {
      //            std::cout << name() << std::endl;
      dispatcher_->update();
    }

    //std::cout << "1.run" << name()<< std::endl;
    execute();
    //std::cout << "2.e-run" << std::endl;
    if (dispatcher_ != nullptr) {
      dispatcher_->remove(this);
      set_dispatcher(nullptr);
    }

    set_executable(false);
    //        std::cout << "After event"<< std::endl;     
  }
}
