/* 
 * File:   IndividualsFileReporter.cpp
 * Author: Merlin
 * 
 * Created on November 7, 2014, 11:06 AM
 */

#include "IndividualsFileReporter.h"
#include "Model.h"
#include "Population/Properties/PersonIndexAll.h"
#include "Population/Person.h"
#include "Population/Population.h"
#include "Core/Config/Config.h"
#include "Core/Scheduler.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/SingleHostClonalParasitePopulations.h"

IndividualsFileReporter::IndividualsFileReporter(const std::string &file_name) : file_name_(file_name) {
}

IndividualsFileReporter::~IndividualsFileReporter() {
}

void IndividualsFileReporter::before_run() {
  fs_.open(file_name_.c_str(), std::fstream::out);
}

void IndividualsFileReporter::begin_time_step() {

}

void IndividualsFileReporter::after_time_step() {
  if (Model::SCHEDULER->current_time()%Model::CONFIG->report_frequency()==0) {
    for (std::size_t i = 0; i < Model::POPULATION->all_persons()->vPerson().size(); i++) {
      Person *person = Model::POPULATION->all_persons()->vPerson()[i];
      double p_density = 0;
      if (person->all_clonal_parasite_populations()->parasites()->size() >= 1) {
        p_density = person->all_clonal_parasite_populations()->parasites()->at(
            0)->last_update_log10_parasite_density();
      } else {
        p_density = Model::CONFIG->parasite_density_level().log_parasite_density_cured;
      }
      fs_ << p_density << "\t";
    }
    fs_ << std::endl;
  }

}

void IndividualsFileReporter::monthly_report() {
}

void IndividualsFileReporter::after_run() {
  fs_.close();
}






