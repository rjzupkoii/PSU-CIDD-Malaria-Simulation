/* 
 * File:   MultipleLocationGuiReporter.h
 * Author: Merlin
 *
 * Created on August 18, 2013, 8:38 PM
 */

#ifndef MULTIPLELOCATIONGUIREPORTER_H
#define    MULTIPLELOCATIONGUIREPORTER_H

#include "Reporters/GuiReporter.h"
#include "Core/PropertyMacro.h"

class MultipleLocationGuiReporter : public GuiReporter {
 DISALLOW_COPY_AND_ASSIGN(MultipleLocationGuiReporter)

 public:
  MultipleLocationGuiReporter();

  //    MultipleLocationGuiReporter(const MultipleLocationGuiReporter& orig);
  virtual ~MultipleLocationGuiReporter();

  virtual void initialize();

  virtual void before_run();

  virtual void after_run();

  virtual void begin_time_step();

  virtual void monthly_report();

  void send_tme_chart_data(const int &location);

 private:

};

#endif    /* MULTIPLELOCATIONGUIREPORTER_H */

