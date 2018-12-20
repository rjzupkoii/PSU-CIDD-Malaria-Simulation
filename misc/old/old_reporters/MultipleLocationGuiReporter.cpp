/* 
 * File:   MultipleLocationGuiReporter.cpp
 * Author: Merlin
 * 
 * Created on August 18, 2013, 8:38 PM
 */

#include "MultipleLocationGuiReporter.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Person.h"

MultipleLocationGuiReporter::MultipleLocationGuiReporter() = default;

MultipleLocationGuiReporter::~MultipleLocationGuiReporter() = default;

void MultipleLocationGuiReporter::initialize() {
  GuiReporter::initialize();
}

void MultipleLocationGuiReporter::before_run() {
  std::cout << "version" << SEPARATOR << "3.0" << std::endl;
  std::cout << "time" << SEPARATOR << Model::CONFIG->total_time() << SEPARATOR << Model::CONFIG->update_frequency()
            << std::endl;

  // TODO: rewrite using fmt
//    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
//        //create first line chart
//        CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % prevalanceChart % location),
//                boost::str(boost::format("Village %1%") % (location + 1)), "Time", "Integer", 0, Model::CONFIG->total_time(), "Prevalance", "Double", 0, 0.1, "EIR per person day", "Double", 0, 0.1);
//    }
//
//    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
//        CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % monoResistanceChart % location),
//                boost::str(boost::format("Mono Resistance for location %1%") % location), "Time", "Integer", 0, Model::CONFIG->total_time(), "Biting-rate weighted FOI", "Double", 0, 1);
//    }
//
//    CreateWindowsTab("NewTab", "List", simulationInfoTab, "Simulation Info");
//    CreateWindowsTab("NewTab", "Tree", simulationEventInfoTab, "Event Info");
//    if (Model::TREATMENT_STRATEGY->get_type() == IStrategy::Cycling || Model::TREATMENT_STRATEGY->get_type() == IStrategy::AdaptiveCycling) {
//        CreateWindowsTableTab("NewTab", "Table", simulationUTLTab, "UTL Duration", "True");
//    }
//
//    SendTabData(simulationInfoTab, "Seed", "", "", NumberToString<int>(Model::RANDOM->seed()), "Seed = <Data>");
//
//    std::stringstream s_beta;
//    std::stringstream s_pop_size;
//    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
//        s_beta << Model::CONFIG->location_db()[loc].beta<< "\t";
//        s_pop_size << Model::CONFIG->location_db()[loc].populationSize<< "\t";
//    }
//
//    SendTabData(simulationInfoTab, "Beta", "", "", s_beta.str(), "Beta = <Data>");
//    SendTabData(simulationInfoTab, "PopulationSize", "", "", s_pop_size.str(), "Population Size = <Data>");
//
//
//    // this hack is to keep legends in chart are in order
//    for (int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++) {
//        std::string chart_name = boost::str(boost::format("%1%%2%") % prevalanceChart % loc);
//        SendLineChartData(chart_name, "qPCR", NumberToString<int>(0), NumberToString<double>(0), "0");
//        SendLineChartData(chart_name, "Microscopy", NumberToString<int>(0), NumberToString<double>(0), "0");
//        SendLineChartData(chart_name, "RDT", NumberToString<int>(0), NumberToString<double>(0), "0");
//
//        //        SendLineChartData(chart_name, "Clinical", NumberToString<int>(0), NumberToString<double>(0), "0");
//        SendLineChartData(chart_name, "EIR", NumberToString<int>(0), NumberToString<int>(0), "1");
//    }
}

void MultipleLocationGuiReporter::after_run() {
}

void MultipleLocationGuiReporter::begin_time_step() {
}

void MultipleLocationGuiReporter::monthly_report() {

  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    for (int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
      send_tme_chart_data(location);
      send_mono_resistance_chart_data(location);
    }
  }
}

void MultipleLocationGuiReporter::send_tme_chart_data(const int &location) {
  // TODO: rewrite using fmt
//  std::string chart_name = boost::str(boost::format("%1%%2%") % prevalanceChart % location);
//
//  double qPCR = (Model::DATA_COLLECTOR->popsize_by_location_hoststate()[location][Person::ASYMPTOMATIC] +
//                 Model::DATA_COLLECTOR->popsize_by_location_hoststate()[location][Person::CLINICAL]) /
//                (double) Model::DATA_COLLECTOR->popsize_by_location()[location];
//  double RDT = 0.1 * qPCR;
//  SendLineChartData(chart_name, "qPCR", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(qPCR), "0");
//  SendLineChartData(chart_name, "Microscopy", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]), "0");
//  SendLineChartData(chart_name, "RDT", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(RDT), "0");
//
//  //    SendLineChartData(chart_name, "Clinical", NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(Model::STATISTIC->fraction_of_positive_that_are_clinical_by_location()[location]), "0");
//
//  //TODO::change it later
//  SendLineChartData(chart_name, "EIR", NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(
//          Model::DATA_COLLECTOR->current_EIR_by_location()[location] / (double) Model::CONFIG->report_frequency()),
//                    "1");
}