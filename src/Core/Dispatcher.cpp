/* 
 * File:   Dispatcher.cpp
 * Author: nguyentran
 * 
 * Created on May 3, 2013, 3:46 PM
 */

#include "Dispatcher.h"
#include "Population/Properties/IndexHandler.hxx"
#include "Events/Event.h"
#include "Helpers/ObjectHelpers.h"

Dispatcher::Dispatcher() : events_(nullptr) {}

// Create the event dispatcher and preallocate some space for future events
void Dispatcher::init() {
  events_ = new EventPtrVector();
  events_->reserve(INTITAL_ALLOCATION);
}

Dispatcher::~Dispatcher() {
  Dispatcher::clear_events();
  ObjectHelpers::delete_pointer<EventPtrVector>(events_);
}

// Add the event to the dispatcher
void Dispatcher::add(Event *event) {
  events_->push_back(event);
  event->IndexHandler::set_index(events_->size() - 1);
}

// Remove the event from the dispatcher
void Dispatcher::remove(Event *event) {
  // Move the given event to the back of the vector
  events_->back()->IndexHandler::set_index(event->IndexHandler::index());
  (*events_)[event->IndexHandler::index()] = events_->back();

  // Remove it from the vector and clear our index
  events_->pop_back();
  event->IndexHandler::set_index(-1);
}

// Disable all events in the dispatcher and then clear it.
void Dispatcher::clear_events() {
  // Return if there is nothing to do
  if (events_ == nullptr) { return; }
  if (events_->empty()) { return; }

  // Disable the events in the dispatcher, slightly faster than an iterator
  for (std::size_t ndx = 0; ndx < events_->size(); ndx++) {
    (*events_)[ndx]->dispatcher = nullptr;
    (*events_)[ndx]->executable = false;
  }

  // Remove the events from the dispatcher
  events_->clear();
}

void Dispatcher::update() {
  events_->shrink_to_fit();
}