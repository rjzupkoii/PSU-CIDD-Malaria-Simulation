//
// Created by ktt5121 on 9/5/2021.
//

#include <Core/Config/Config.h>
#include "PRMC.h"
#include "Model.h"
#include "Population/Population.h"
#include "Population/Properties/PersonIndexAll.h"
#include <Population/Properties/PersonIndexByLocationStageBittingLevel.h>
#include <map>
#include <math.h>
#include <tuple>
#include <vector>
#include <iterator>
#include <numeric>

PRMC::PRMC() {
}

PRMC::~PRMC() = default;

/* Get data from config */
void PRMC::addInterruptedFeedingRateByLocation(int location, double if_rate){
  if(interrupted_feeding_rate.empty()){
    interrupted_feeding_rate = std::vector<double>(Model::CONFIG->number_of_locations());
  }
  if(interrupted_feeding_rate_from_config.empty()){
    interrupted_feeding_rate_from_config = std::vector<double>(Model::CONFIG->number_of_locations());
  }
  interrupted_feeding_rate[location] = if_rate;
  interrupted_feeding_rate_from_config[location] = if_rate;
}

/* Init PRMC */
void PRMC::init(){
  /* Init PRMC table */
  if(genotypes_table.empty()){
    std::vector<Genotype*> genotypes (size(),nullptr);
    genotypes_table = std::vector<std::vector<std::vector<Genotype*>>>(Model::CONFIG->number_of_locations());
    for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
      genotypes_table[loc] = std::vector<std::vector<Genotype*>>(Model::CONFIG->number_of_tracking_days());
      for(int day = 0; day < Model::CONFIG->number_of_tracking_days(); day++){
        genotypes_table[loc][day] = genotypes;
      }
    }
    genotypes_table_to_random = genotypes_table;
  }

  /* Data for debug - current genotype stats */
  current_recombined_genotypes_stats = std::vector<std::map<int, int>>(Model::CONFIG->number_of_locations());

  /* Data for debug - tracking genotype stats */
  tracking_recombined_genotypes_stats = std::vector<std::vector<std::map<int, int>>>(Model::CONFIG->number_of_locations());
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    tracking_recombined_genotypes_stats[loc] = std::vector<std::map<int, int>>(Model::CONFIG->number_of_tracking_days());
  }
  /* Second way to infect a person, check in population.cpp */
  tracking_recombined_genotypes_stats_to_infect = tracking_recombined_genotypes_stats;

  /* Data for reporter */
  recombined_genotype_stats = std::vector<std::map<std::string,int>>(Model::CONFIG->number_of_locations());
  population_recombined_genotype_stats = std::vector<std::map<std::string,int>>(Model::CONFIG->number_of_locations());

  //Initialize all population genotypes
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    recombined_genotype_stats[loc] = std::map<std::string,int>();
    population_recombined_genotype_stats[loc] = std::map<std::string,int>();
  }

  /* Initialize all possible genotypes */
  possible_recombined_genotype_labels = std::vector<std::vector<std::string>>(Model::CONFIG->number_of_locations());
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    possible_recombined_genotype_labels[loc] = std::vector<std::string>();
  }
  /* Find all genotypes that can be recombined from config */
  calculatePossibleRecombinedGenotypes();

  infected_persons = std::vector<Person*>();
  all_persons = std::vector<Person*>();
  br_gd_vector = std::vector<double>();
  br_vector = std::vector<double>();
  starting_time_ = (date::sys_days(starting_date_) - date::sys_days(Model::CONFIG->starting_date())).count();
  ending_time_ = (date::sys_days(ending_date_) - date::sys_days(Model::CONFIG->starting_date())).count();

  /* Parse starting and ending date from config */
  VLOG(0) << "[PRMC] PRMC starting date: " << starting_date() << " starting time: " << starting_time_;
  VLOG(0) << "[PRMC] PRMC ending date: " << ending_date() << " ending time: " << ending_time_;
  if(starting_date() < Model::CONFIG->starting_date()){
      starting_date_ = Model::CONFIG->starting_date();
      VLOG(0) << "[PRMC] Date for interrupted feeding earlier than model date, set to model date";
  }
  if(ending_date() > Model::CONFIG->ending_date()){
      ending_date_ = Model::CONFIG->ending_date();
      VLOG(0) << "[PRMC] Date for interrupted feeding latter than model date, set to model date";
  }
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
      if(interrupted_feeding_rate[loc] == 0.0){
          VLOG(0) << "[PRMC] Interrupted feeding rate is zero, will ignore the interrupted feature at location " << loc;
      }
  }
  VLOG(0) << "[PRMC] Initialized PRMC";
}

/* Init reporter */
void PRMC::initReporter(int job_number, std::string path) {
  /* Generate daily reporter only if enabled in config */
  if(generate_daily_report()) {
    daily_reporter_prmc = new DailyReporterPRMC();
    daily_reporter_prmc->initialize(job_number, path);
    VLOG(0) << "[PRMC] Initialized PRMC daily reporter";
    daily_reporter_population_genotypes = new DailyReporterPopulationGenotypes();
    daily_reporter_population_genotypes->initialize(job_number,path);
    VLOG(0) << "[PRMC] Initialized PRMC daily population genotype reporter";
  }
  else{
    VLOG(0) << "[PRMC] Will not generate daily genotype reporter";
  }
}

/* Report daily (if enabled) */
void PRMC::reportDaily(){
  updateGenotypeStats();
  daily_reporter_prmc->reportDaily(recombined_genotype_stats);
  updatePopulationGenotypeStats();
  daily_reporter_population_genotypes->reportDaily(population_recombined_genotype_stats);
}

/* Update PRMC genotype stats */
void PRMC::updateGenotypeStats(){
  for(int location = 0; location < Model::CONFIG->number_of_locations(); location++){
    for(auto genotype : current_recombined_genotypes_stats[location]){
      if(genotype.first != -1){
        recombined_genotype_stats[location][Model::CONFIG->genotype_db()->at(genotype.first)->to_string()] = genotype.second;}
      else{
        recombined_genotype_stats[location]["-1"] = genotype.second;
        VLOG(0) << "   [PRMC][updateReport] PRMC genotype " << genotype.first << ": " << genotype.second;
      }
    }
  }
}

/* Update population genotype stats */
void PRMC::updatePopulationGenotypeStats(){
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationBittingLevel>();
  for(int location = 0; location < Model::CONFIG->number_of_locations(); location++){
    for(int biting_level = 0; biting_level < pi->vPerson()[location].size(); biting_level++){
      if(pi->vPerson()[location][biting_level].size() > 0){
        for(int index = 0; index < pi->vPerson()[location][biting_level].size(); index++){
          for(auto parasite : *pi->vPerson()[location][biting_level][index]->all_clonal_parasite_populations()->parasites()){
            population_recombined_genotype_stats[location][parasite->genotype()->to_string()] = population_recombined_genotype_stats[location][parasite->genotype()->to_string()] + 1;
            //                        VLOG(0) << "[PRMC][updateReportPopulationGenotypes] person id " << pi->vPerson()[location][biting_level][index]->get_uid()
            //                                        << " genotype " << parasite->genotype()->to_string();
          }
        }
      }
    }
  }
}

/* Sample population */
void PRMC::sample(const int &current_time){
  this->current_time = current_time;
  reset();
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    if(current_time < starting_time_ || current_time > ending_time_){
      interrupted_feeding_rate[loc] = 0.0;
    }
    else{
      interrupted_feeding_rate[loc] = interrupted_feeding_rate_from_config[loc];
    }
  }
  infectNewCohortInPRMC();
}

/* Update PRMC */
void PRMC::update(const int &current_time){
  genotypes_table_to_random = genotypes_table;
  tracking_recombined_genotypes_stats_to_infect = tracking_recombined_genotypes_stats;
  /* DEBUG */
  /*
  VLOG(0) << "[PRMC][updateDaily] genotypes_table_to_random = genotypes_table";
  VLOG(0) << "[PRMC][updateDaily] tracking_recombined_genotypes_stats_to_infect = tracking_recombined_genotypes_stats";
  printCurrentDayInfo();
  printCurrentDayStatsInfo();
  printTrackingDayInfo();
  printTrackingDayStatsInfo();
  */
  if(generate_daily_report()){
    reportDaily();
  }
}

/* Reset PRMC data before new day */
void PRMC::reset(){
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    current_recombined_genotypes_stats[loc] = std::map<int, int>();
    for(auto& map : recombined_genotype_stats[loc]){
      map.second = 0;
    }
    for(auto& map : population_recombined_genotype_stats[loc]){
      map.second = 0;
    }
  }
}

/* Get all combination of a set of numbers */
std::vector<std::vector<int>> PRMC::getCombinationOfNumbers(std::vector<int> numbers){
  /* https://stackoverflow.com/a/27461228 */
  std::vector<std::vector<int>> all_combinations;
  int n = numbers.size();
  int cap = 1 << n;
  for( int i=1; i<cap; ++i )
  {
    std::vector<int> combination;
    for( int j=0; j<n; ++j )
    {
      if( i & (1<<j) ){
        combination.push_back(numbers[j]);
      }
    }
    all_combinations.push_back(combination);
  }
  return all_combinations;
}

/* Get all recombined genotypes */
std::vector<std::string> PRMC::getRecombinedGenotypeStrings(std::string genotype1, std::string genotype2){
  std::vector<std::string> new_genotypes;
  if(genotype1 == genotype2){
    new_genotypes.push_back(genotype1);
  }
  else{
    std::vector<int> combined_allele_positions;
    std::string combined_string = "";
    for(int k = 0; k < genotype1.length(); k++){
      combined_string += "0";
    }
    for(int k = 0; k < genotype1.length(); k++){
      if(genotype1[k] != genotype2[k]){
        combined_allele_positions.push_back(k);
        combined_string[k] = '1';
      }
    }
    std::vector<std::vector<int>> combined_position_arrays = getCombinationOfNumbers(combined_allele_positions);
    for(int i = 0; i < combined_position_arrays.size(); i++){
      //            VLOG(0) << "    [PRMC][recombineGenotypes] combined position array position " << i;
      std::string genotype1_temp = genotype1;
      std::string genotype2_temp = genotype2;
      //            VLOG(0) << "    [PRMC][recombineGenotypes] genotype1       " << genotype1_temp;
      //            VLOG(0) << "    [PRMC][recombineGenotypes] genotype2       " << genotype2_temp;
      //            VLOG(0) << "    [PRMC][recombineGenotypes] combined string " << combined_string;
      for(int j = 0; j < combined_position_arrays[i].size(); j++){
        const char temp = genotype1_temp[combined_position_arrays[i][j]];
        //                VLOG(0) << "      [PRMC][recombineGenotypes] combined position array position value " << combined_position_arrays[i][j];
        //                VLOG(0) << "        [PRMC][recombineGenotypes] swap " << genotype1_temp[combined_position_arrays[i][j]] << " of g1 to " << genotype2_temp[combined_position_arrays[i][j]] << " of g2";
        //                VLOG(0) << "        [PRMC][recombineGenotypes] swap " << genotype2_temp[combined_position_arrays[i][j]] << " of g2 to " << temp << " of g1";
        genotype1_temp[combined_position_arrays[i][j]] = genotype2_temp[combined_position_arrays[i][j]];
        genotype2_temp[combined_position_arrays[i][j]] = temp;
      }
      //            VLOG(0) << "      [PRMC][recombineGenotypes] swapped genotype1 " << genotype1_temp;
      //            VLOG(0) << "      [PRMC][recombineGenotypes] swapped genotype2 " << genotype2_temp;
      new_genotypes.push_back(genotype1_temp);
      new_genotypes.push_back(genotype2_temp);
    }
  }
  /* DEBUG */
  /*
  for(auto genotype : new_genotypes){
      VLOG(0) << "   [PRMC][recombineGenotypes] new genotype " << genotype;
  }
   */
  return new_genotypes;
}

/* Find all possible recombined genotypes and add to genotype stats*/
void PRMC::calculatePossibleRecombinedGenotypes() {
  std::vector<std::vector<Genotype*>> will_recombined_genotypes = std::vector<std::vector<Genotype*>>(Model::CONFIG->number_of_locations());
  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    will_recombined_genotypes[loc] = std::vector<Genotype*>();
  }

  for(int loc = 0; loc < Model::CONFIG->number_of_locations(); loc++){
    for(const auto p_info : Model::CONFIG->initial_parasite_info()) {
      auto *genotype = Model::CONFIG->genotype_db()->at(p_info.parasite_type_id);
      if(p_info.location == loc){
//        VLOG(0) << "[PRMC][calculatePossibleRecombinedGenotypes] location " << loc << " has genotypes " << genotype->to_string();
        will_recombined_genotypes[loc].push_back(genotype);
      }
    }
  }

  for(int loc = 0; loc < will_recombined_genotypes.size(); loc++){
    for(int i = 0; i < will_recombined_genotypes[loc].size(); i++){
      for(int j = 0; j < will_recombined_genotypes[loc].size(); j++) {
        auto* genotype_i = will_recombined_genotypes[loc][i];
        auto* genotype_j = will_recombined_genotypes[loc][j];
//        VLOG(0) << "  [PRMC][calculatePossibleRecombinedGenotypes] location " << loc << " genotype i: " << i << " " << genotype_i->to_string() <<" combine with genotype j: " << j << " " << genotype_j->to_string();
        auto recombined_genotypes = getRecombinedGenotypeStrings(genotype_i->to_string(),genotype_j->to_string());
        for(auto recombined_genotype_string : recombined_genotypes){
          if(possible_recombined_genotype_labels[loc].empty()){
            possible_recombined_genotype_labels[loc].push_back(recombined_genotype_string);
//            VLOG(0) << "  [PRMC][calculatePossibleRecombinedGenotypes] first genotype " << recombined_genotype_string << ", added";
          }
          else{
            bool not_existed = true;
            for(auto current_genotype_label : possible_recombined_genotype_labels[loc]){
              not_existed = not_existed && (current_genotype_label != recombined_genotype_string);
//              if(current_genotype_label == recombined_genotype_string){
//                  VLOG(0) << "  [PRMC][calculatePossibleRecombinedGenotypes] existed genotype " << recombined_genotype_string << ", ignored";
//              }
            }
            if(not_existed){
              possible_recombined_genotype_labels[loc].push_back(recombined_genotype_string);
//              VLOG(0) << "  [PRMC][calculatePossibleRecombinedGenotypes] new genotype " << recombined_genotype_string << ", added";
            }
          }
        }
      }
    }
  }

  for(int loc = 0; loc < possible_recombined_genotype_labels.size(); loc++){
    std::map<std::string,int> possible_genotype_stats;
    std::map<std::string,int> population_genotype_stat;
    for(int index = 0; index < possible_recombined_genotype_labels[loc].size(); index++){
      possible_genotype_stats.insert_or_assign(possible_recombined_genotype_labels[loc][index],0);
      population_genotype_stat.insert_or_assign(possible_recombined_genotype_labels[loc][index],0);
    }
    //Insert the last genotype as nullptr, in case the PRMC position has no infected person
    possible_recombined_genotype_labels[loc].push_back("-1");
    possible_genotype_stats.insert_or_assign("-1",0);
    recombined_genotype_stats[loc] = possible_genotype_stats;
    population_recombined_genotype_stats[loc] = population_genotype_stat;
  }

  /* DEBUG */
  /*
  for(int loc = 0; loc < recombined_genotype_stats.size(); loc++){
      VLOG(0) << "[PRMC][calculatePossibleRecombinedGenotypes] Location " << loc;
      std::map<std::string,int> possible_genotype_stats;
      for(auto genotype : recombined_genotype_stats[loc]){
          VLOG(0) << "  [PRMC][calculatePossibleRecombinedGenotypes] possible genotypes " << genotype.first << ": " << genotype.second;
      }
  }
  */
}

/* Add a person to sample set */
void PRMC::addSamplingPerson(Person* person){
  if(sampling_persons.empty()){
    sampling_persons = std::vector<Person*>();
  }
  if(sampling_genotypes.empty()){
    sampling_genotypes = std::vector<Genotype*>();
  }
  sampling_persons.push_back(person);
}

/* Get person relative infectivity */
double PRMC::getGenotypeRelativeInfectivity(double log_10_density) {
  const auto d_n = log_10_density * Model::CONFIG->relative_infectivity().sigma +
                   Model::CONFIG->relative_infectivity().ro_star;
  const auto p = Model::RANDOM->cdf_standard_normal_distribution(d_n);
  return p * p + 0.01;
}

void PRMC::addNewGenotype(int location, int position, Genotype *genotype) {
  int index_to_add = current_time % (Model::CONFIG->number_of_tracking_days());
  if(genotype != nullptr){
    //        VLOG(0) << " [PRMC][addNewGenotype] Location " << location << " current day " << current_time << " index to add is " << index_to_add << " position " << position << " genotype " << genotype->to_string();
    genotypes_table[location][index_to_add][position] = genotype;
  }
  else{
    //        VLOG(0) << " [PRMC][addNewGenotype] Location " << location << " current day " << current_time << " index_to_add " << index_to_add << " position " << position << " genotype " << "nullptr";
    genotypes_table[location][index_to_add][position] = nullptr;
  }
  Model::DATA_COLLECTOR->set_prmc_genotypes_by_location(genotypes_table);

  if(genotype == nullptr){
    if(current_recombined_genotypes_stats[location].count(-1) == 0)
    {
      current_recombined_genotypes_stats[location].insert_or_assign(-1,1);
      tracking_recombined_genotypes_stats[location][index_to_add].insert_or_assign(-1,1);
      //            VLOG(0) << "[PRMC][addNewGenotype][CurrentStats] New genotype, insert genotype " << genotype->to_string();
    } else {
      current_recombined_genotypes_stats[location][-1] = current_recombined_genotypes_stats[location][-1] + 1;
      tracking_recombined_genotypes_stats[location][index_to_add][-1] = tracking_recombined_genotypes_stats[location][index_to_add][-1] + 1;
      //            VLOG(0) << "[PRMC][addNewGenotype][CurrentStats] Existed genotype " << genotype->to_string() << ", update count to " << current_recombined_genotypes_stats[location][genotype->to_string()];
    }
  }
  else{
    if(current_recombined_genotypes_stats[location].count(genotype->genotype_id()) == 0)
    {
      current_recombined_genotypes_stats[location].insert_or_assign(genotype->genotype_id(),1);
      tracking_recombined_genotypes_stats[location][index_to_add].insert_or_assign(genotype->genotype_id(),1);
      //            VLOG(0) << "[PRMC][addNewGenotype][CurrentStats] New genotype, insert genotype " << genotype->to_string();
    } else {
      current_recombined_genotypes_stats[location][genotype->genotype_id()] = current_recombined_genotypes_stats[location][genotype->genotype_id()] + 1;
      tracking_recombined_genotypes_stats[location][index_to_add][genotype->genotype_id()] = tracking_recombined_genotypes_stats[location][index_to_add][genotype->genotype_id()] + 1;
      //            VLOG(0) << "[PRMC][addNewGenotype][CurrentStats] Existed genotype " << genotype->to_string() << ", update count to " << current_recombined_genotypes_stats[location][genotype->to_string()];
    }
  }
}
/* Get recombined genotypes from person/people */
void PRMC::infectNewCohortInPRMC(){
  auto* pi = Model::POPULATION->get_person_index<PersonIndexByLocationStageBittingLevel>();

  /* Get infected people from biting index */
  for(int location = 0; location < Model::CONFIG->number_of_locations(); location++){
    for(std::size_t hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++){
      for(int biting_level = 0; biting_level < pi->vPerson()[location][hs].size(); biting_level++){
        if(hs == Person::ASYMPTOMATIC || hs == Person::CLINICAL)
        {
          //Just care about biting level has persons, to make sure bite infected persons, add to infected person vector
          if(pi->vPerson()[location][hs][biting_level].size() > 0){
            std::vector<Person*> infected_persons_each_level;
            for(int index = 0; index < pi->vPerson()[location][hs][biting_level].size(); index++){
              if(std::isnan(pi->vPerson()[location][hs][biting_level][index]->current_relative_infectivity())
                  || std::isinf(pi->vPerson()[location][hs][biting_level][index]->current_relative_infectivity())
                  || std::isnan(pi->vPerson()[location][hs][biting_level][index]->get_biting_level_value())
                  || std::isinf(pi->vPerson()[location][hs][biting_level][index]->get_biting_level_value())){
                VLOG(0) << " [PRMC][infectNewCohortInPRMC] Person relative biting value " << pi->vPerson()[location][hs][biting_level][index]->get_biting_level_value();
                VLOG(0) << " [PRMC][infectNewCohortInPRMC] Person log10 density " << pi->vPerson()[location][hs][biting_level][index]->all_clonal_parasite_populations()->log10_total_relative_density();
                VLOG(0) << " [PRMC][infectNewCohortInPRMC] Person g(d) pre calc " << pi->vPerson()[location][hs][biting_level][index]->current_relative_infectivity();
                VLOG(0) << " [PRMC][infectNewCohortInPRMC] Person g(d) calc " << pi->vPerson()[location][hs][biting_level][index]->relative_infectivity(pi->vPerson()[location][hs][biting_level][index]->all_clonal_parasite_populations()->log10_total_relative_density());
              }
              infected_persons_each_level.push_back(pi->vPerson()[location][hs][biting_level][index]);
              infected_persons.push_back(pi->vPerson()[location][hs][biting_level][index]);
              br_gd_vector.push_back(pi->vPerson()[location][hs][biting_level][index]->current_relative_infectivity() * pi->vPerson()[location][hs][biting_level][index]->get_biting_level_value());
            }
          }
        }
        /* Add other persons to all person set */
        if(pi->vPerson()[location][hs][biting_level].size() > 0)
        {
          for(int index = 0; index < pi->vPerson()[location][hs][biting_level].size(); index++) {
            all_persons.push_back(pi->vPerson()[location][hs][biting_level][index]);
            br_vector.push_back(pi->vPerson()[location][hs][biting_level][index]->get_biting_level_value());
          }
        }
      }
    }
    //        VLOG(0) << " [PRMC][infectNewCohortInPRMC] Location " << location << " total infected persons are " << infected_persons.size();
    if(infected_persons.empty()) {
      //            VLOG(0) << " [PRMC][infectNewCohortInPRMC] infected_persons.empty()";
      for(int position = 0; position < size(); position++) {
        addNewGenotype(location,position,nullptr);
      }
      continue;
    }
    /* Sampling person/people */
    for(int position = 0; position < size(); position++) {
      //            VLOG(0) << "  [PRMC][infectNewCohortInPRMC] Location " << location << " Position " << position;
      //Make multinomial array
      std::vector<unsigned int> number_of_bites_each_level(infected_persons.size());
      bool interrupted_feeding = Model::RANDOM->random_flat(0.0, 1.0) < interrupted_feeding_rate[location] ? true : false;
      if(interrupted_feeding){
        //Select 1 person first
        Model::RANDOM->random_multinomial(infected_persons.size(), 1,
                                          &br_gd_vector[0],
                                          &number_of_bites_each_level[0]);
        //                    VLOG(0) << " [PRMC][infectNewCohortInPRMC] sum of number_of_bites_each_level " << std::accumulate(number_of_bites_each_level.begin(), number_of_bites_each_level.end(), 0);;
        for(int i = 0; i < number_of_bites_each_level.size(); i++) {
          if(number_of_bites_each_level[i] != 0){
            for(int j = 0; j < number_of_bites_each_level[i]; j++){
              //                            VLOG(0) << "   [PRMC][infectNewCohortInPRMC] Sampling first person " << (j+1) << " index " << i << " id "  << infected_persons[i]->get_uid() << " is Gametocytemia " << infected_persons[i]->isGametocytaemic() ? " True" : "False";
              //                            if(infected_persons[i]->isGametocytaemic())
              {
                //                                VLOG(0) << "   [PRMC][infectNewCohortInPRMC] Sampling first person " << (j+1) << " index " << i << " id "  << infected_persons[i].person->get_uid()
                //                                        << " parasite size " << infected_persons[i].person->all_clonal_parasite_populations()->parasites()->size();
                //                                for(auto parasite : *infected_persons[i].person->all_clonal_parasite_populations()->parasites()){
                //                                    VLOG(0) << "    [PRMC][infectNewCohortInPRMC] Parasite genotype: " << parasite->genotype()->to_string();
                //                                }
                addSamplingPerson(infected_persons[i]);
              }
            }
          }
        }
        //if interrupted feeding, select one more person (uninfected or not)
        number_of_bites_each_level = std::vector<unsigned int>(all_persons.size());
        Model::RANDOM->random_multinomial(all_persons.size(), 1,
                                          &br_vector[0],
                                          &number_of_bites_each_level[0]);
        for(int i = 0; i < number_of_bites_each_level.size(); i++) {
          //                    VLOG(0) <<"   [PRMC][infectNewCohortInPRMC] second person host state: " << all_persons[i].person->host_state() << " total parasites: " << all_persons[i].person->all_clonal_parasite_populations()->size();
          //If second person is infected, check g(d)
          if(number_of_bites_each_level[i] != 0){
            //                                VLOG(0) <<"   [PRMC][infectNewCohortInPRMC] second infected person total parasites: " << all_persons[i].person->all_clonal_parasite_populations()->size();
            for(int j = 0; j < number_of_bites_each_level[i]; j++){
              //Check g(density) to decide to sample person or not
              //                            if(all_persons[i]->isGametocytaemic())
              {
                if(Model::RANDOM->random_flat(0.0, 1.0) > all_persons[i]->current_relative_infectivity())
                {
                  addSamplingPerson(all_persons[i]);
                  //                                        VLOG(0) << "   [PRMC][infectNewCohortInPRMC] IF enabled, IF true, Sampling second person id "  << all_persons[i].person->get_uid()
                  //                                            << " parasite size " << all_persons[i].person->all_clonal_parasite_populations()->parasites()->size();
                  //                                        for(auto parasite : *all_persons[i].person->all_clonal_parasite_populations()->parasites()){
                  //                                            VLOG(0) << "    [PRMC][infectNewCohortInPRMC] Parasite genotype: " << parasite->genotype()->to_string();
                  //                                        }
                }
              }
            }
          }
        }
        samplingGenotypeMultinomial(location,position);
      }
      else{
        number_of_bites_each_level = std::vector<unsigned int>(infected_persons.size());
        //Interrupted feeding enabled but not interrupted, select 1 person only
        Model::RANDOM->random_multinomial(infected_persons.size(), 1,
                                          &br_gd_vector[0],
                                          &number_of_bites_each_level[0]);
        //                    VLOG(0) << " [PRMC][infectNewCohortInPRMC] sum of number_of_bites_each_level " << std::accumulate(number_of_bites_each_level.begin(), number_of_bites_each_level.end(), 0);;
        for(int i = 0; i < number_of_bites_each_level.size(); i++) {
          if(number_of_bites_each_level[i] != 0){
            for(int j = 0; j < number_of_bites_each_level[i]; j++){
              //                                VLOG(0) << "   [PRMC][infectNewCohortInPRMC] IF enabled, IF false, Sampling one person " << (j+1) << " index " << i << " id "  << infected_persons[i].person->get_uid()
              //                                        << " parasite size " << infected_persons[i].person->all_clonal_parasite_populations()->parasites()->size();
              //                                for(auto parasite : *infected_persons[i].person->all_clonal_parasite_populations()->parasites()){
              //                                    VLOG(0) << "    [PRMC][infectNewCohortInPRMC] Parasite genotype: " << parasite->genotype()->to_string();
              //                                }
              addSamplingPerson(infected_persons[i]);
            }
          }
        }
        samplingGenotypeMultinomial(location,position);
      }
    }
    infected_persons.clear();
    all_persons.clear();
    br_gd_vector.clear();
    br_vector.clear();
  }
}

/* Recombine genotypes using genotype loci */
Genotype* PRMC::recombineGenotypes(Genotype* first_genotype, Genotype* second_genotype){
  if(first_genotype->genotype_id() == second_genotype->genotype_id()){
    return first_genotype;
  }
  std::vector<int> combined_genotype_expression;
  //Get recombined genotype loci from parents, 50% chance
  for(int loci = 0; loci < first_genotype->gene_expression().size(); loci++){
    if(Model::RANDOM->random_flat(0.0,1.0) >= 0.5){
      combined_genotype_expression.push_back(second_genotype->gene_expression()[loci]);
    }
    else{
      combined_genotype_expression.push_back(first_genotype->gene_expression()[loci]);
    }
  }
  //    VLOG(0) << "    [PRMC][recombineGenotypes] After combination, new genotype expression is " << Model::CONFIG->genotype_db()->at(Model::CONFIG->genotype_db()->get_id(combined_genotype_expression))->to_string();
  return Model::CONFIG->genotype_db()->at(Model::CONFIG->genotype_db()->get_id(combined_genotype_expression));
}

/* Sampling genotypes from infected people */
void PRMC::samplingGenotypeMultinomial(int location, int position){
  if(sampling_persons.empty()) {
    //        VLOG(0) << "   [PRMC][samplingGenotypeMultinomial] sampling_persons.empty()";
    addNewGenotype(location, position, nullptr);
    return;
  }
  //Add all parasites/genotypes to a pool
  std::vector<double> relative_infectivity_each_parasite;
  for(auto person : sampling_persons) {
    //        VLOG(0) << " [PRMC][samplingGenotypeMultinomial] Location " << person->location() << ", sampling person " << person->get_uid();
    const int total_parasites = person->all_clonal_parasite_populations()->parasites()->size();
    //        VLOG(0) << " [PRMC][samplingGenotypeMultinomial] Person id " << person->get_uid() << " has " << total_parasites << " parasites";
    int parasite_index = 0;
    for(auto *parasite: *person->all_clonal_parasite_populations()->parasites()) {
      sampling_genotypes.push_back(parasite->genotype());
      relative_infectivity_each_parasite.push_back(getGenotypeRelativeInfectivity(parasite->get_current_parasite_density(current_time)));
//            VLOG(0) << "  [PRMC][samplingGenotypeMultinomial] Available genotype are " << parasite_index << ": " << parasite->genotype()->to_string() << " with g(d) " <<  getGenotypeRelativeInfectivity(parasite->get_current_parasite_density(current_time));
      parasite_index++;
    }
  }
  if(sampling_genotypes.empty()) {
    //        VLOG(0) << "  [PRMC][samplingGenotypeMultinomial] sampling_genotypes.empty()";
    addNewGenotype(location, position, nullptr);
    return;
  }

  if(sampling_genotypes.size() == 1){
    addNewGenotype(sampling_persons.back()->location(), position, sampling_genotypes.back());
  }
  else{
    //Select two genotypes from all genotypes pool
    std::vector<unsigned int> selected_genotypes_nomial(relative_infectivity_each_parasite.size());
    Model::RANDOM->random_multinomial(relative_infectivity_each_parasite.size(), 2,&relative_infectivity_each_parasite[0], &selected_genotypes_nomial[0]);
    std::vector<Genotype*> will_recombine_genotypes;
    for(int i = 0; i < selected_genotypes_nomial.size(); i++){
      if(selected_genotypes_nomial[i] != 0){
        //                VLOG(0) << " [PRMC][samplingGenotypeMultinomial] Select genotype " << i << ": " << selected_genotypes_nomial[i] << " times, string is " << sampling_genotypes[i].genotype->to_string() << " with g(d) " <<  relative_infectivity_each_parasite[i];
        for(int j = 0; j < selected_genotypes_nomial[i]; j++){
          will_recombine_genotypes.push_back(sampling_genotypes[i]);
          //                    VLOG(0) << "   [PRMC][samplingGenotypeMultinomial] Pushed genotype " << i << ": " << (j+1) << " times, string is " << sampling_genotypes[i].genotype->to_string() << " with g(d) " <<  relative_infectivity_each_parasite[i] << " to recombine array";
        }
      }
    }
    //Recombine 2 genotypes and add to PRMC
    addNewGenotype(sampling_persons.back()->location(), position, recombineGenotypes(will_recombine_genotypes.front(),will_recombine_genotypes.back()));
    will_recombine_genotypes.clear();
  }
  sampling_genotypes.clear();
  sampling_persons.clear();
  relative_infectivity_each_parasite.clear();
}

/* Getter */
std::vector<std::map<std::string,int>> PRMC::getRecombinedGenotypeStats() {
  return recombined_genotype_stats;
}

/* Getter */
std::vector<std::map<std::string,int>> PRMC::getPopulationRecombinedGenotypeStats() {
  return population_recombined_genotype_stats;
}

/* Second way to infect */
std::map<int,int> PRMC::getTrackingGenotypeStatsByLocationAndDay(int location, int tracking_day){
  if(current_time < Model::CONFIG->number_of_tracking_days()) {
    return tracking_recombined_genotypes_stats[location][tracking_day];
  }
  else{
    return tracking_recombined_genotypes_stats_to_infect[location][tracking_day];
  }
}

/* First way to infect */
Genotype* PRMC::getRandomGenotype(std::size_t loc, int tracking_day){
  auto genotype_index = Model::RANDOM->random_uniform_int(0,size());
  if(current_time < Model::CONFIG->number_of_tracking_days()) {
    //        VLOG(0) <<"[PRMC][getRandomGenotypeFromPRMC] current day is " << current_time << " tracking day is " << tracking_day << ", index to get is " << genotype_index << ", random index is " << genotype_index << " from genotype_table";
    return genotypes_table[loc][tracking_day][genotype_index];
  }
  else{
    //        VLOG(0) <<"[PRMC][getRandomGenotypeFromPRMC] current day is " << current_time << " tracking day is " << tracking_day << ", index to get is " << genotype_index << ", random index is " << genotype_index << " from genotype_table_random";
    return genotypes_table_to_random[loc][tracking_day][genotype_index];
  }
}

/* Print all population recombined genotype headers */
void PRMC::printPopulationGenotypeHeader(std::stringstream& ss){
  for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    int index = 0;
    for(std::pair<std::string,int> stats : population_recombined_genotype_stats[location]){
      ss << location << ":" << stats.first;
      if(index < population_recombined_genotype_stats[location].size() - 1){
        ss << Csv::sep;
      }
      index++;
    }
    if(location < Model::CONFIG->number_of_locations() - 1){
      ss << Csv::sep;
    }
  }
}

/* Print all population recombined genotype data */
void PRMC::printPopulationGenotypeStats(std::stringstream& ss){
  if(!generate_daily_report()){
    updatePopulationGenotypeStats();
  }
  for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    int index = 0;
    for(auto stats : population_recombined_genotype_stats[location]){
      ss << stats.second;
      if(index < population_recombined_genotype_stats[location].size() - 1){
        ss << Csv::sep;
      }
      index++;
    }
    if(location < Model::CONFIG->number_of_locations() - 1){
      ss << Csv::sep;
    }
  }
}

/* Print all recombined genotype headers */
void PRMC::printRecombinedGenotypeHeader(std::stringstream& ss){
  for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    int index = 0;
    for(std::pair<std::string,int> stats : recombined_genotype_stats[location]){
      ss << location << ":" << stats.first;
      if(index < recombined_genotype_stats[location].size() - 1){
        ss << Csv::sep;
      }
      index++;
    }
    if(location < Model::CONFIG->number_of_locations() - 1){
      ss << Csv::sep;
    }
  }
}

/* Print all recombined genotype headers */
void PRMC::printRecombinedGenotypeStats(std::stringstream& ss){
  if(!generate_daily_report()){
    updateGenotypeStats();
  }
  for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
    int index = 0;
    for(auto stats : recombined_genotype_stats[location]){
      ss << stats.second;
      if(index < recombined_genotype_stats[location].size() - 1){
        ss << Csv::sep;
      }
      index++;
    }
    if(location < Model::CONFIG->number_of_locations() - 1){
      ss << Csv::sep;
    }
  }
}

/* DEBUG */
void PRMC::printCurrentDayInfo(){
  if(genotypes_table.empty()) return;
  VLOG(0) << "[PRMC][printCurrentDayInfo] Current day " << current_time;
  for(int location = 0; location < genotypes_table.size(); location++){
    VLOG(0) << " [PRMC][printCurrentDayInfo] Location " << location;
    int index = current_time % Model::CONFIG->number_of_tracking_days();
    for(auto genotype : genotypes_table[location][index]){
      if(genotype == nullptr){
        VLOG(0) << "   [PRMC][printTrackingDayInfo] PRMC genotype " << "nullptr";
      }
      else{
        VLOG(0) << "   [PRMC][printTrackingDayInfo] PRMC genotype " << genotype->to_string();
      }
    }
  }
}

void PRMC::printTrackingDayInfo(){
  if(genotypes_table.empty()) return;
  VLOG(0) << " [PRMC][printTrackingDayInfo] Current day " << current_time;
  for(int location = 0; location < genotypes_table.size(); location++){
    VLOG(0) << " [PRMC][printTrackingDayInfo] Location " << location;
    for(int index = 0; index < genotypes_table[location].size(); index++){
      if(genotypes_table[location][index].empty()){
        //              VLOG(0) << " [PRMC][printTrackingDayInfo] PRMC genotype not added yet";
        continue;
      }
      VLOG(0) << "  [PRMC][printTrackingDayInfo] Index " << index << " (" << current_time - index << " days ago)";
      for(auto genotype : genotypes_table[location][index]){
        if(genotype == nullptr){
          VLOG(0) << "   [PRMC][printTrackingDayInfo] PRMC genotype " << "nullptr";
        }
        else{
          VLOG(0) << "   [PRMC][printTrackingDayInfo] PRMC genotype " << genotype->to_string();
        }
      }
    }
  }
}

void PRMC::printCurrentDayStatsInfo(){
  if(genotypes_table.empty()) return;
  VLOG(0) << "[PRMC][printCurrentDayStatsInfo] Current day " << current_time;
  for(int location = 0; location < current_recombined_genotypes_stats.size(); location++){
    VLOG(0) << " [PRMC][printCurrentDayStatsInfo] Day " << current_time << " Location " << location;
    for(auto genotype : current_recombined_genotypes_stats[location]){
      if(genotype.first != -1){
        VLOG(0) << "   [PRMC][printCurrentDayStatsInfo] PRMC genotype " << Model::CONFIG->genotype_db()->at(genotype.first)->to_string() << ": " << genotype.second;
      }
      else{
        VLOG(0) << "   [PRMC][printCurrentDayStatsInfo] PRMC genotype " << genotype.first << ": " << genotype.second;
      }
    }
  }
}

void PRMC::printTrackingDayStatsInfo(){
  if(genotypes_table.empty()) return;
  VLOG(0) << "[PRMC][printTrackingDayStatsInfo] Current day " << current_time;
  //    for(int location = 0; location < tracking_recombined_genotypes_stats.size(); location++){
  for(int location = 0; location < tracking_recombined_genotypes_stats.size(); location++){
    VLOG(0) << " [PRMC][printTrackingDayStatsInfo] Location " << location;
    for(int index = 0; index < tracking_recombined_genotypes_stats[location].size(); index++){
      if(tracking_recombined_genotypes_stats[location][index].empty()){
        //                VLOG(0) << " [PRMC][printTrackingDayStatsInfo] PRMC genotype not added yet";
        continue;
      }
      if(current_time % Model::CONFIG->number_of_tracking_days() == index){
        VLOG(0) << "  [PRMC][printTrackingDayStatsInfo] Index " << index << " day " << current_time << " <<==";
      }
      else{
        VLOG(0) << "  [PRMC][printTrackingDayStatsInfo] Index " << index;
      }
      for(auto genotype : tracking_recombined_genotypes_stats[location][index]){
        if(genotype.first != -1){
          VLOG(0) << "   [PRMC][printTrackingDayStatsInfo] PRMC genotype " << Model::CONFIG->genotype_db()->at(genotype.first)->to_string() << ": " << genotype.second;
        }
        else{
          VLOG(0) << "   [PRMC][printTrackingDayStatsInfo] PRMC genotype " << genotype.first << ": " << genotype.second;
        }
      }
    }
  }
}