/* 
 * File:   Event.cpp
 * Author: nguyentran
 * 
 * Created on May 3, 2013, 3:13 PM
 */

#include "Event.h"
#include  "Core/Dispatcher.h"

Event::Event() = default;

Event::~Event() {
  if (dispatcher!=nullptr) {
    dispatcher->remove(this);
  }
  dispatcher = nullptr;
  scheduler = nullptr;
}

void Event::perform_execute() {
  if (executable) {
    //        std::cout << "Before event"<< std::endl;
    //update to current status of the dispatcher
    if (dispatcher!=nullptr) {
      //            std::cout << name() << std::endl;
      dispatcher->update();
    }

    //std::cout << "1.run" << name()<< std::endl;
    execute();
    //std::cout << "2.e-run" << std::endl;
    if (dispatcher!=nullptr) {
      dispatcher->remove(this);
      dispatcher = nullptr;
    }

    executable = false;
    //        std::cout << "After event"<< std::endl;
  }
}
