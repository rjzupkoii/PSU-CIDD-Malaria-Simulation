/* 
 * File:   GuiReporter.cpp
 * Author: Merlin
 * 
 * Created on August 17, 2013, 4:40 PM
 */

#include "GuiReporter.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Strategies/IStrategy.h"

std::string GuiReporter::prevalanceChart = "PrevalenceChart";
std::string GuiReporter::ageClassChart = "AgeClassChart";
std::string GuiReporter::meanMOIChart = "MeanMOIChart";
std::string GuiReporter::multiInfectionChart = "MuiltiInfectionChart";

std::string GuiReporter::monoResistanceChart = "MonoResistanceChart";
std::string GuiReporter::doubleResistanceChart = "DoubleResistanceChart";
std::string GuiReporter::tripleResistanceChart = "TrippleResistanceChart";

std::string GuiReporter::simulationInfoTab = "SimulationInfo";
std::string GuiReporter::simulationEventInfoTab = "EventInfo";
std::string GuiReporter::simulationUTLTab = "UTLInfo";

std::map<std::string, GuiReporter::CommandEnum>
    GuiReporter::CommandsMap = std::map<std::string, GuiReporter::CommandEnum>();

GuiReporter::GuiReporter() {
}

GuiReporter::~GuiReporter() {
}

void GuiReporter::initialize() {
  CommandsMap["con"] = ContinueCommand;
  CommandsMap["pau"] = PauseCommand;
  CommandsMap["sto"] = StopCommand;
}

void GuiReporter::before_run() {
  // TODO: rewrite using fmt

//    std::cout << "version" << SEPARATOR << "3.0" << std::endl;
//    std::cout << "time" << SEPARATOR << Model::CONFIG->total_time() << SEPARATOR << Model::CONFIG->update_frequency() << std::endl;
//
//
//    CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % prevalanceChart % 0), "Prevalance & Immune", "Time", "Integer", 0, Model::CONFIG->total_time(), "Prevalance", "Double", 0, 0.1, "Immune level", "Double", 0, 0.3);
//
//    CreateNewChart("BarChart", boost::str(boost::format("%1%%2%") % ageClassChart % 0), "Age Class", "Age Class", "Category", 0, 0, "Percent", "Double", 0, 1);
//
//    CreateNewChart("BarChart", boost::str(boost::format("%1%%2%") % multiInfectionChart % 0), "Multi Infection Chart", "Number of Parasite Population", "Category", 0, 0, "Number of Person", "Integer", 0, 1);
//
//
//    CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % monoResistanceChart % 0), "Mono Resistance", "Time", "Integer", 0, Model::CONFIG->total_time(), "Biting-rate weighted FOI", "Double", 0, 1);
//    CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % doubleResistanceChart % 0), "Double Resistance", "Time", "Integer", 0, Model::CONFIG->total_time(), "Biting-rate weighted FOI", "Double", 0, 1);
//    CreateNewChart("LineChart", boost::str(boost::format("%1%%2%") % tripleResistanceChart % 0), "Triple Resistance", "Time", "Integer", 0, Model::CONFIG->total_time(), "Biting-rate weighted FOI", "Double", 0, 1);
//
//
//    CreateWindowsTab("NewTab", "List", simulationInfoTab, "Simulation Info");
//    CreateWindowsTab("NewTab", "Tree", simulationEventInfoTab, "Event Info");
//    if (Model::TREATMENT_STRATEGY->get_type() == IStrategy::Cycling || Model::TREATMENT_STRATEGY->get_type() == IStrategy::AdaptiveCycling) {
//        CreateWindowsTableTab("NewTab", "Table", simulationUTLTab, "UTL Duration", "True");
//    }
//
//    SendTabData(simulationInfoTab, "Seed", "", "", NumberToString<int>(Model::RANDOM->seed()), "Seed = <Data>");
//    SendTabData(simulationInfoTab, "Beta", "", "", NumberToString<double>(Model::CONFIG->location_db()[0].beta), "Beta = <Data>");
//    SendTabData(simulationInfoTab, "PopulationSize", "", "", NumberToString<int>(Model::CONFIG->location_db()[0].populationSize), "Population Size = <Data>");

}

void GuiReporter::after_run() {
  std::cout << std::endl;

}

void GuiReporter::begin_time_step() {
  std::cout << "Command?" << std::endl;

  std::string command;
  std::cin >> command;

  while (command!="con") {
    //xu ly command
    HandleGuiCommand(command);
    std::cin >> command;
  }

}

void GuiReporter::HandleGuiCommand(const std::string &command) {

  if (CommandsMap.find(command)==CommandsMap.end()) {
    std::cout << "invalid command" << std::endl;
  } else {
    switch (CommandsMap[command]) {
      case PauseCommand:std::cout << "Pause" << std::endl;
        break;
      case StopCommand:Model::SCHEDULER->set_is_force_stop(true);
        break;
      case ContinueCommand:break;
    }
  }
}

void GuiReporter::monthly_report() {
  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    //        Model::DATA_COLLECTOR->perform_population_statistic();

    send_prevalence_chart_data_for_location(0);
    //        send_age_class_chart_data(0);
    send_multiple_infection_chart_data(0);
    send_mono_resistance_chart_data(0);
    send_double_resistance_chart_data(0);
    send_triple_resistance_chart_data(0);

    send_mean_MOI_chart_data(0);

  }

}

void
GuiReporter::CreateNewChart(const std::string &chartType, const std::string &chartName, const std::string &chartTitle,
                            const std::string &xAxisTitle, const std::string &xAxisType, const int &xRangeFrom,
                            const int &xRangeTo, const std::string &y1AxisTitle, const std::string &y1AxisType,
                            const double &y1RangeFrom, const double &y1RangeTo, const std::string &y2AxisTitle,
                            const std::string &y2AxisType, const double &y2RangeFrom, const double &y2RangeTo) {
// TODO: rewrite using fmt

  //  if (y2AxisTitle == "") {
//    std::cout <<
//              boost::str(boost::format("%1%|%2%|%3%|%4%|%5%|%6%|%7%|%8%|%9%|%10%|%11%|%12%|")
//                         % "ChartNew"
//                         % chartType
//                         % chartName
//                         % chartTitle
//                         % xAxisTitle
//                         % xAxisType
//                         % xRangeFrom
//                         % xRangeTo
//                         % y1AxisTitle
//                         % y1AxisType
//                         % y1RangeFrom
//                         % y1RangeTo
//              ) << std::endl;
//  } else {
//    std::cout <<
//              boost::str(boost::format("%1%|%2%|%3%|%4%|%5%|%6%|%7%|%8%|%9%|%10%|%11%|%12%|%13%|%14%|%15%|%16%|")
//                         % "ChartNew"
//                         % chartType
//                         % chartName
//                         % chartTitle
//                         % xAxisTitle
//                         % xAxisType
//                         % xRangeFrom
//                         % xRangeTo
//                         % y1AxisTitle
//                         % y1AxisType
//                         % y1RangeFrom
//                         % y1RangeTo
//                         % y2AxisTitle
//                         % y2AxisType
//                         % y2RangeFrom
//                         % y2RangeTo
//              ) << std::endl;
//  }
  //
  //    std::cout << "ChartNew" << SEPARATOR;
  //    std::cout << chartType << SEPARATOR;
  //    std::cout << chartName << SEPARATOR;
  //    std::cout << chartTitle << SEPARATOR;
  //    std::cout << xAxisTitle << SEPARATOR;
  //    std::cout << xAxisType << SEPARATOR;
  //    std::cout << xRangeFrom << SEPARATOR;
  //    std::cout << xRangeTo << SEPARATOR;
  //    std::cout << y1AxisTitle << SEPARATOR;
  //    std::cout << y1AxisType << SEPARATOR;
  //    std::cout << y1RangeFrom << SEPARATOR;
  //    std::cout << y1RangeTo << SEPARATOR;
  //    if (y2AxisTitle != "") {
  //        std::cout << y2AxisTitle << SEPARATOR;
  //        std::cout << y2AxisType << SEPARATOR;
  //        std::cout << y2RangeFrom << SEPARATOR;
  //        std::cout << y2RangeTo << SEPARATOR;
  //    }
  //
  //    std::cout << std::endl;
}

void GuiReporter::CreateWindowsTab(const std::string &command, const std::string &tabType, const std::string &tabName,
                                   const std::string &tabTitle) {
  std::cout << command << SEPARATOR <<
            tabType << SEPARATOR <<
            tabName << SEPARATOR <<
            tabTitle << std::endl;
}

void
GuiReporter::CreateWindowsTableTab(const std::string &command, const std::string &tabType, const std::string &tabName,
                                   const std::string &tabTitle, const std::string &switchRowWithColumn) {
  std::cout << command << SEPARATOR <<
            tabType << SEPARATOR <<
            tabName << SEPARATOR <<
            tabTitle << SEPARATOR <<
            switchRowWithColumn << std::endl;
}

void GuiReporter::SendLineChartData(const std::string &chartName, const std::string &legend, const std::string &xValue,
                                    const std::string &yValue, const std::string &yAxis) {
  std::cout << "ChartData" << SEPARATOR <<
            chartName << SEPARATOR <<
            legend << SEPARATOR <<
            "" << SEPARATOR <<
            xValue << SEPARATOR <<
            yValue << SEPARATOR <<
            yAxis << std::endl;
}

void GuiReporter::SendBarChartData(const std::string &chartName, const std::string &category, const std::string &legend,
                                   const std::string &xValue, const std::string &yValue, const std::string &yAxis) {
  std::cout << "ChartData" << SEPARATOR <<
            chartName << SEPARATOR <<
            category << SEPARATOR <<
            legend << SEPARATOR <<
            xValue << SEPARATOR <<
            yValue << SEPARATOR <<
            yAxis << std::endl;
}

void GuiReporter::SendTabData(const std::string &tabName, const std::string &group, const std::string &category,
                              const std::string &time, const std::string &data, const std::string &format) {
  std::cout << "TabData" << SEPARATOR <<
            tabName << SEPARATOR <<
            group << SEPARATOR <<
            category << SEPARATOR <<
            time << SEPARATOR <<
            data << SEPARATOR <<
            format << std::endl;
}

void GuiReporter::SendTabTableData(const std::string &tabName, const std::string &header, const std::string &data) {
  std::cout << "TabData" << SEPARATOR <<
            tabName << SEPARATOR <<
            header << SEPARATOR <<
            data << std::endl;
}

void GuiReporter::send_prevalence_chart_data_for_location(const int &location) {
// TODO: rewrite using fmt
//  std::string chart_name = boost::str(boost::format("%1%%2%") % prevalanceChart % location);
//  //    SendLineChartData(chart_name, "Blood Slide", NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>((Model::STATISTIC->popsize_by_location_hoststate()[location][Person::ASYMPTOMATIC] + Model::STATISTIC->popsize_by_location_hoststate()[location][Person::CLINICAL]) / (double) Model::STATISTIC->popsize_by_location()[location]), "0");
//  SendLineChartData(chart_name, "Blood Slide", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location]), "0");
//  SendLineChartData(chart_name, "Clinical", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(
//                            Model::DATA_COLLECTOR->fraction_of_positive_that_are_clinical_by_location()[location]),
//                    "0");
//  SendLineChartData(chart_name, "Immune Level", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(Model::DATA_COLLECTOR->total_immune_by_location()[location] /
//                                           Model::DATA_COLLECTOR->popsize_by_location()[location]), "1");
}

void GuiReporter::send_age_class_chart_data(const int &location) {

}

void GuiReporter::send_multiple_infection_chart_data(const int &location) {
  // TODO: rewrite using fmt
//  std::string chart_name = boost::str(boost::format("%1%%2%") % multiInfectionChart % location);
//  SendBarChartData(chart_name, "1", "Number of Infection", NumberToString<int>(Model::SCHEDULER->current_time()),
//                   NumberToString<int>(Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location][0]), "0");
//  SendBarChartData(chart_name, "2", "Number of Infection", NumberToString<int>(Model::SCHEDULER->current_time()),
//                   NumberToString<int>(Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location][1]), "0");
//  SendBarChartData(chart_name, "3", "Number of Infection", NumberToString<int>(Model::SCHEDULER->current_time()),
//                   NumberToString<int>(Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location][2]), "0");
//  SendBarChartData(chart_name, ">3", "Number of Infection", NumberToString<int>(Model::SCHEDULER->current_time()),
//                   NumberToString<int>(Model::DATA_COLLECTOR->multiple_of_infection_by_location()[location][3]), "0");
}

void GuiReporter::send_mono_resistance_chart_data(const int &location) {
// TODO: rewrite using fmt
//  std::string mono_chart_name = boost::str(boost::format("%1%%2%") % monoResistanceChart % location);
//  //    std::string double_chart_name = boost::str(boost::format("%1%%2%") % doubleResistanceChart % location);
//
//  SendLineChartData(mono_chart_name, "Non Resistance", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(
//                            Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][0]),
//                    "0");
//
//
//  //    DrugTypePtrMap::iterator it;
//  //    for (it = Model::CONFIG->drug_db()->drug_db().begin(); it != Model::CONFIG->drug_db()->drug_db().end(); it++) {
//  //        int d_id = it->second->id();
//  //        double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][d_id];
//  //        num = (num < 0) ? 0 : num;
//  //
//  //        SendLineChartData(mono_chart_name, NumberToString<int>(d_id), NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
//  //    }
//  GenotypePtrMap::iterator it;
//  for (it = Model::CONFIG->genotype_db()->db().begin(); it != Model::CONFIG->genotype_db()->db().end(); it++) {
//    if (it->second->number_of_resistance_position() == 1) {
//      int d_id = it->second->genotype_id();
//      double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][d_id];
//      num = (num < 0) ? 0 : num;
//
//      SendLineChartData(mono_chart_name, NumberToString<int>(d_id),
//                        NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
//    }

//  }


  //    DrugTypePtrMap::iterator it1;
  //    for (it1 = Model::CONFIG->drug_db()->drug_db().begin(); it1 != Model::CONFIG->drug_db()->drug_db().end(); it++) {
  //        DrugTypePtrMap::iterator it2;
  //        for (it2 = it1; it2 != Model::CONFIG->drug_db()->drug_db().end(); it++) {
  //            if (it2 != it1) {
  //                boost::dynamic_bitset<> double_res = (it1->second->resistance_bit_string() | it2->second->resistance_bit_string());
  //                int res_id = double_res.to_ulong();
  //                double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][res_id];
  //                num = (num < 0) ? 0 : num;
  //
  //                SendLineChartData(double_chart_name, NumberToString<int>(res_id), NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
  //
  //            }
  //        }
  //    }

  //    for (int i = 0; i < Global::strategy->drugTypeList.size() - 1; i++) {
  //        int ptype1 = Global::strategy->drugTypeList[i]->id;
  //        for (int j = i + 1; j < Global::strategy->drugTypeList.size(); j++) {
  //            int ptype2 = Global::strategy->drugTypeList[j]->id;
  //            string sType = toString(ptype1) + "-" + toString(ptype2);
  //            int doubleResistanceType = ptype1 | ptype2;
  //            int num = Population::currentNumberOfParasitesByLocationType[0][doubleResistanceType];
  //            num = (num < 0) ? 0 : num;
  //            Report::SendLineChartData(Report::doubleResistanceChart, sType, toString(currentTime), toString(num), "0");
  //        }
  //    }
}

void GuiReporter::send_double_resistance_chart_data(const int &location) {
// TODO: rewrite using fmt
  //    std::string mono_chart_name = boost::str(boost::format("%1%%2%") % monoResistanceChart % location);

//  std::string double_chart_name = boost::str(boost::format("%1%%2%") % doubleResistanceChart % location);
//
//  //    SendLineChartData(double_chart_name, "Non Resistance", NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][0]), "0");
//
//
//  //    GenotypePtrMap::iterator it;
//  GenotypePtrMap::iterator it;
//  //    for (it = Model::CONFIG->parasite_db()->genotype_db().begin(); it != Model::CONFIG->parasite_db()->genotype_db().end(); it++) {
//  for (it = Model::CONFIG->genotype_db()->db().begin(); it != Model::CONFIG->genotype_db()->db().end(); it++) {
//    if (it->second->number_of_resistance_position() == 2) {
//      int d_id = it->second->genotype_id();
//      double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][d_id];
//      num = (num < 0) ? 0 : num;
//
//      SendLineChartData(double_chart_name, NumberToString<int>(d_id),
//                        NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
//    }
//
//  }


  //    DrugTypePtrMap::iterator it1;
  //    for (it1 = Model::CONFIG->drug_db()->drug_db().begin(); it1 != Model::CONFIG->drug_db()->drug_db().end(); it++) {
  //        DrugTypePtrMap::iterator it2;
  //        for (it2 = it1; it2 != Model::CONFIG->drug_db()->drug_db().end(); it++) {
  //            if (it2 != it1) {
  //                boost::dynamic_bitset<> double_res = (it1->second->resistance_bit_string() | it2->second->resistance_bit_string());
  //                int res_id = double_res.to_ulong();
  //                double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][res_id];
  //                num = (num < 0) ? 0 : num;
  //
  //                SendLineChartData(double_chart_name, NumberToString<int>(res_id), NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
  //
  //            }
  //        }
  //    }

  //    for (int i = 0; i < Global::strategy->drugTypeList.size() - 1; i++) {
  //        int ptype1 = Global::strategy->drugTypeList[i]->id;
  //        for (int j = i + 1; j < Global::strategy->drugTypeList.size(); j++) {
  //            int ptype2 = Global::strategy->drugTypeList[j]->id;
  //            string sType = toString(ptype1) + "-" + toString(ptype2);
  //            int doubleResistanceType = ptype1 | ptype2;
  //            int num = Population::currentNumberOfParasitesByLocationType[0][doubleResistanceType];
  //            num = (num < 0) ? 0 : num;
  //            Report::SendLineChartData(Report::doubleResistanceChart, sType, toString(currentTime), toString(num), "0");
  //        }
  //    }
}

void GuiReporter::send_triple_resistance_chart_data(const int &location) {
// TODO: rewrite using fmt
  //    std::string mono_chart_name = boost::str(boost::format("%1%%2%") % monoResistanceChart % location);
//  std::string double_chart_name = boost::str(boost::format("%1%%2%") % tripleResistanceChart % location);
//
//  //    SendLineChartData(double_chart_name, "Non Resistance", NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][0]), "0");
//
//
//  //    GenotypePtrMap::iterator it;
//  GenotypePtrMap::iterator it;
//  for (it = Model::CONFIG->genotype_db()->db().begin(); it != Model::CONFIG->genotype_db()->db().end(); it++) {
//    if (it->second->number_of_resistance_position() == 3) {
//      int d_id = it->second->genotype_id();
//      double num = Model::POPULATION->current_force_of_infection_by_location_parasite_type()[location][d_id];
//      num = (num < 0) ? 0 : num;
//
//      SendLineChartData(double_chart_name, NumberToString<int>(d_id),
//                        NumberToString<int>(Model::SCHEDULER->current_time()), NumberToString<double>(num), "0");
//    }
//
//  }

}

void GuiReporter::send_mean_MOI_chart_data(const int &location) {
// TODO: rewrite using fmt
//  std::string double_chart_name = boost::str(boost::format("%1%%2%") % meanMOIChart % location);
//  SendLineChartData(double_chart_name, "Mean MOI", NumberToString<int>(Model::SCHEDULER->current_time()),
//                    NumberToString<double>(boost::accumulators::mean(Model::DATA_COLLECTOR->acc)), "0");

}