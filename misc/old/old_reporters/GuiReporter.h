/* 
 * File:   GuiReporter.h
 * Author: Merlin
 *
 * Created on August 17, 2013, 4:40 PM
 */

#ifndef GUIREPORTER_H
#define    GUIREPORTER_H

#include "Reporter.h"
#include "Core/PropertyMacro.h"
#include <map>

#ifndef SEPARATOR
#define SEPARATOR "|"
#endif

class GuiReporter : public Reporter {
 public:

  enum CommandEnum {
    ContinueCommand, PauseCommand, StopCommand
  };

 public:
  static std::string prevalanceChart;
  static std::string ageClassChart;
  static std::string meanMOIChart;
  static std::string multiInfectionChart;

  static std::string monoResistanceChart;
  static std::string doubleResistanceChart;
  static std::string tripleResistanceChart;

  static std::string simulationInfoTab;
  static std::string simulationEventInfoTab;
  static std::string simulationUTLTab;

  static std::map<std::string, CommandEnum> CommandsMap;

 DISALLOW_COPY_AND_ASSIGN(GuiReporter)

 public:
  GuiReporter();

  //    GuiReporter(const GuiReporter &orig);
  virtual ~GuiReporter();

  virtual void initialize();

  virtual void before_run();

  virtual void after_run();

  virtual void begin_time_step();

  virtual void monthly_report();

  void CreateNewChart(const std::string &chartType, const std::string &chartName, const std::string &chartTitle,
                      const std::string &xAxisTitle, const std::string &xAxisType, const int &xRangeFrom,
                      const int &xRangeTo, const std::string &y1AxisTitle, const std::string &y1AxisType,
                      const double &y1RangeFrom, const double &y1RangeTo, const std::string &y2AxisTitle = "",
                      const std::string &y2AxisType = "", const double &y2RangeFrom = 0, const double &y2RangeTo = 0);

  void CreateWindowsTab(const std::string &command, const std::string &tabType, const std::string &tabName,
                        const std::string &tabTitle);

  void CreateWindowsTableTab(const std::string &command, const std::string &tabType, const std::string &tabName,
                             const std::string &tabTitle, const std::string &switchRowWithColumn);

  void SendLineChartData(const std::string &chartName, const std::string &legend, const std::string &xValue,
                         const std::string &yValue, const std::string &yAxis);

  void SendBarChartData(const std::string &chartName, const std::string &category, const std::string &legend,
                        const std::string &xValue, const std::string &yValue, const std::string &yAxis);

  void SendTabData(const std::string &tabName, const std::string &group, const std::string &category,
                   const std::string &time, const std::string &data, const std::string &format);

  void SendTabTableData(const std::string &tabName, const std::string &header, const std::string &data);

  void HandleGuiCommand(const std::string &command);

  void send_prevalence_chart_data_for_location(const int &location);

  void send_age_class_chart_data(const int &location);

  void send_multiple_infection_chart_data(const int &location);

  void send_mono_resistance_chart_data(const int &location);

  void send_double_resistance_chart_data(const int &location);

  void send_triple_resistance_chart_data(const int &location);

  void send_mean_MOI_chart_data(const int &location);

 private:

};

#endif    /* GUIREPORTER_H */

