/**
 * PopulationEventBuilder.cpp
 *
 * Implement the population event builder factory.
 */
#include "PopulationEventBuilder.h"

#include <algorithm>
#include <vector>

#include "yaml-cpp/yaml.h"
#include "Core/Config/Config.h"
#include "ImportationEvent.h"
#include "ImportationPeriodicallyEvent.h"
#include "ChangeTreatmentCoverageEvent.h"
#include "ChangeStrategyEvent.h"
#include "Model.h"
#include "SingleRoundMDAEvent.h"
#include "ModifyNestedMFTEvent.h"
#include "TurnOnMutationEvent.h"
#include "TurnOffMutationEvent.h"
#include "IntroducePlas2CopyParasiteEvent.h"
#include "IntroduceAQMutantEvent.h"
#include "IntroduceLumefantrineMutantEvent.h"
#include "GIS/SpatialData.h"

#include "AnnualBetaUpdateEvent.hxx"
#include "AnnualCoverageUpdateEvent.hxx"
#include "ImportationPeriodicallyRandomEvent.h"
#include "IntroduceMutantEvent.h"

std::vector<Event*> PopulationEventBuilder::build_introduce_parasite_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    int location = node[i]["location"].as<int>();
    if (static_cast<std::size_t>(location) < config->number_of_locations()) {
      for (std::size_t j = 0; j < node[i]["parasite_info"].size(); j++) {
        auto genotype_id = node[i]["parasite_info"][j]["genotype_id"].as<int>();
        auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = node[i]["parasite_info"][j]["day"].as<date::year_month_day>();
        auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

        auto* event = new ImportationEvent(location, time, genotype_id, num);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*>
PopulationEventBuilder::build_introduce_parasites_periodically_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;

  for (std::size_t i = 0; i < node.size(); i++) {
    const unsigned long location = node[i]["location"].as<unsigned long>();
    const unsigned long location_from = location;
    const auto location_to = std::min(location + 1, config->number_of_locations());

    for (auto loc = location_from; loc < location_to; ++loc) {
      for (std::size_t j = 0; j < node[i]["parasite_info"].size(); j++) {
        //            InitialParasiteInfo ipi;
        //            ipi.location = location;
        const auto genotype_id = -1;
        // TODO: implement new importation parasite genotype based on allele distribution


        const auto dur = node[i]["parasite_info"][j]["duration"].as<int>();
        const auto num = node[i]["parasite_info"][j]["number_of_cases"].as<int>();

        const auto starting_date = node[i]["parasite_info"][j]["start_day"].as<date::year_month_day>();
        auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

        auto* event = new ImportationPeriodicallyEvent(loc, dur, genotype_id, num, time);
        events.push_back(event);
      }
    }
  }
  return events;
}

std::vector<Event*>
PopulationEventBuilder::build_change_treatment_coverage_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    auto* tcm = ITreatmentCoverageModel::build(node[i], config);
    // std::cout << tcm->starting_time << std::endl;
    auto* e = new ChangeTreatmentCoverageEvent(tcm);
    events.push_back(e);
  }
  return events;
}

std::vector<Event*>
PopulationEventBuilder::build_change_treatment_strategy_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
    auto strategy_id = node[i]["strategy_id"].as<int>();

    // Verify that the strategy id is valid, if not fail
    if (strategy_id >= config->strategy_db.value_.size()) {
      LOG(ERROR) << "Invalid strategy_id! " << strategy_id << " supplied, but strategy_db size is " << config->strategy_db.value_.size();
      exit(-1);
    }

    auto* e = new ChangeStrategyEvent(time, strategy_id);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_single_round_mda_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
    auto* e = new SingleRoundMDAEvent(time);
    for (std::size_t loc = 0; loc < config->number_of_locations(); loc++) {
      auto input_loc = node[i]["fraction_population_targeted"].size() < config->number_of_locations() ? 0 : loc;
      e->fraction_population_targeted.push_back(node[i]["fraction_population_targeted"][input_loc].as<double>());
    }

    e->days_to_complete_all_treatments = node[i]["days_to_complete_all_treatments"].as<int>();
    events.push_back(e);
  }

  return events;
}

std::vector<Event*>
PopulationEventBuilder::build_modify_nested_mft_strategy_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
    auto strategy_id = node[i]["strategy_id"].as<int>();

    auto* e = new ModifyNestedMFTEvent(time, strategy_id);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*> PopulationEventBuilder::build_turn_on_mutation_event(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event*> events;
    for (std::size_t i = 0; i < node.size(); i++) {
      const auto starting_date = node[i]["day"].as<date::year_month_day>();
      auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
      double mutation_probability = node[i]["mutation_probability"]
                                      ? node[i]["mutation_probability"].as<double>()
                                      : Model::CONFIG->drug_db()->at(0)->p_mutation();

      int drug_id = node[i]["drug_id"] ? node[i]["drug_id"].as<int>() : -1;

      auto* e = new TurnOnMutationEvent(time, mutation_probability, drug_id);
      events.push_back(e);
    }

    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in turn_on_mutation node: " << error.msg;
    exit(1);
  }
}

std::vector<Event*> PopulationEventBuilder::build_turn_off_mutation_event(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    const auto starting_date = node[i]["day"].as<date::year_month_day>();
    auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();
    auto* e = new TurnOffMutationEvent(time);
    events.push_back(e);
  }

  return events;
}

std::vector<Event*>
PopulationEventBuilder::build_introduce_plas2_parasite_events(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    int location = node[i]["location"].as<int>();
    if (static_cast<std::size_t>(location) < config->number_of_locations()) {
      auto fraction = node[i]["fraction"].as<double>();

      const auto starting_date = node[i]["day"].as<date::year_month_day>();
      auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

      auto* event = new IntroducePlas2CopyParasiteEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_aq_mutant_parasite_events(const YAML::Node& node,
                                                                                      Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    int location = node[i]["location"].as<int>();
    if (static_cast<std::size_t>(location) < config->number_of_locations()) {
      auto fraction = node[i]["fraction"].as<double>();

      const auto starting_date = node[i]["day"].as<date::year_month_day>();
      auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

      auto* event = new IntroduceAQMutantEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

std::vector<Event*> PopulationEventBuilder::build_introduce_lumefantrine_mutant_parasite_events(const YAML::Node& node,
                                                                                                Config* config) {
  std::vector<Event*> events;
  for (std::size_t i = 0; i < node.size(); i++) {
    int location = node[i]["location"].as<int>();
    if (static_cast<std::size_t>(location) < config->number_of_locations()) {
      auto fraction = node[i]["fraction"].as<double>();

      const auto starting_date = node[i]["day"].as<date::year_month_day>();
      auto time = (date::sys_days{starting_date} - date::sys_days{config->starting_date()}).count();

      auto* event = new IntroduceLumefantrineMutantEvent(location, time, fraction);
      events.push_back(event);
    }
  }
  return events;
}

// Generate a new annual event that adjusts the beta at each location within the model, assumes that the
// YAML node contains a rate of change and start date.
std::vector<Event*> PopulationEventBuilder::build_annual_beta_update_event(const YAML::Node& node, Config* config) {  
  try {
    // Check the node size
    verify_single_node(node, AnnualBetaUpdateEvent::EventName);

    // Build the event
    auto start_date = node[0]["day"].as<date::year_month_day>();
    auto rate = node[0]["rate"].as<float>();
    auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
    auto* event = new AnnualBetaUpdateEvent(rate, time);

    // Log and add the event to the queue, only one for the country
    VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", rate: " << rate;
    std::vector<Event*> events;
    events.push_back(event);
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << AnnualBetaUpdateEvent::EventName << " node: " << error.msg;
    exit(1);
  }
}

// Generate a new annual event that adjusts the coverage at each location within the model, assumes that
// the YAML node contains a rate of change and start date.
std::vector<Event*> PopulationEventBuilder::build_annual_coverage_update_event(const YAML::Node& node, Config* config) {
  try {
    // Check the node size
    verify_single_node(node, AnnualCoverageUpdateEvent::EventName);

    // Build the event
    auto start_date = node[0]["day"].as<date::year_month_day>();
    auto rate = node[0]["rate"].as<float>();
    auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
    auto* event = new AnnualCoverageUpdateEvent(rate, time);

    // Log and add the event to the queue, only one for the country
    VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", rate: " << rate;
    std::vector<Event*> events;
    events.push_back(event);
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << AnnualCoverageUpdateEvent::EventName << " node: " << error.msg;
    exit(1);
  }
}

// Generate a new importation periodically random event that uses a weighted 
// random selection to add a new malaria infection with a specific genotype
// to the model.
std::vector<Event*> PopulationEventBuilder::build_importation_periodically_random_event(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event*> events;
    for (const auto & entry : node) {
      // Load the values
      auto start_date = entry["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
      auto genotype_id = entry["genotype_id"].as<int>();
      auto count = entry["count"].as<int>();
      auto log_parasite_density = entry["log_parasite_density"].as<double>();

      // Check to make sure the date is valid
      if (start_date.day() != date::day{1}) {
        LOG(ERROR) << "Event must start on the first of the month for " << ImportationPeriodicallyRandomEvent::EventName;
        throw std::invalid_argument("Event must start on the first of the month");
      }

      // Double check that the genotype id is valid
      if (genotype_id < 0) {
        LOG(ERROR) << "Invalid genotype id supplied for " << ImportationPeriodicallyRandomEvent::EventName 
                   << " genotype id cannot be less than zero";
        throw std::invalid_argument("Genotype id cannot be less than zero");
      }
      if (genotype_id >= Model::CONFIG->genotype_db()->size()) {
        LOG(ERROR) << "Invalid genotype id supplied for " << ImportationPeriodicallyRandomEvent::EventName 
                   << " genotype id cannot be greater than " << Model::CONFIG->genotype_db()->size() - 1;
        throw std::invalid_argument("Genotype id cannot be greater than genotype_db size");
      }

      // Make sure the count makes sense
      if (count < 1) {
        LOG(ERROR) << "The count of importations must be greater than zero for " << ImportationPeriodicallyRandomEvent::EventName;
        throw std::invalid_argument("Count must be greater than zero");
      }

      // Make sure the log parasite density makes sense
      if (log_parasite_density == 0) {
        LOG(ERROR) << "Invalid log parasite density supplied for " << ImportationPeriodicallyRandomEvent::EventName
                   << "Log10 of zero is undefined.";
        throw std::invalid_argument("Log10 of zero is undefined.");
      }

      // Log and add the event to the queue
      auto* event = new ImportationPeriodicallyRandomEvent(genotype_id, time, count, log_parasite_density);
      VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", genotype_id: " << genotype_id << ", log_parasite_density: " << log_parasite_density;
      events.push_back(event);
    }
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << ImportationPeriodicallyRandomEvent::EventName << " node: " << error.msg;
    exit(1);
  }
}

 std::vector<Event*> PopulationEventBuilder::build_introduce_mutant_events(const YAML::Node& node, Config* config) {
  try {
    std::vector<Event*> events;
    for (const auto & entry : node) {
      // Load the values
      auto start_date = entry["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date} - date::sys_days{config->starting_date()}).count();
      auto district = entry["district"].as<int>();
      auto fraction = entry["fraction"].as<double>();
      auto locus = entry["locus"].as<int>();
      auto mutant_allele = entry["mutant_allele"].as<int>();

      // Make sure the district GIS data is loaded and the district id makes sense
      if (district < 0) {
        LOG(ERROR) << "The target district must be greater than or equal to zero.";
        throw std::invalid_argument("Target district must be greater than or equal to zero");
      }
      if (district > SpatialData::get_instance().get_district_count()) {
        LOG(ERROR) << "Target district is greater than the district count.";
        throw std::invalid_argument("Target district greater than district count.");
      }

      // Make sure the fraction makes sense
      if (fraction <= 0) {
        LOG(ERROR) << "The fraction of the mutants must be greater than zero.";
        throw std::invalid_argument("Mutant fraction must be greater than zero");
      }

      // Make sure the locus and alleles make sense
      if (locus < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Locus must be greater than or equal to zero");
      }
      if (mutant_allele < 0) {
        LOG(ERROR) << "The locus must be greater than or equal to zero.";
        throw std::invalid_argument("Mutant allele must be greater than or equal to zero");
      }

      // Log and add the event to the queue
      auto* event = new IntroduceMutantEvent(time, district, fraction, locus, mutant_allele);
      VLOG(1) << "Adding " << event->name() << " start: " << start_date << ", district: " << district
              << ", locus: " << locus << ", mutant_allele: " << mutant_allele << ", fraction: " << fraction;
      events.push_back(event);
    }
    return events;
  } catch (YAML::BadConversion &error) {
    LOG(ERROR) << "Unrecoverable error parsing YAML value in " << AnnualCoverageUpdateEvent::EventName << " node: " << error.msg;
    exit(1);
  }
}

std::vector<Event*> PopulationEventBuilder::build(const YAML::Node& node, Config* config) {
  std::vector<Event*> events;
  const auto name = node["name"].as<std::string>();

  if (name == "introduce_plas2_parasites") {
    events = build_introduce_plas2_parasite_events(node["info"], config);
  }
  if (name == "introduce_aq_mutant_parasites") {
    events = build_introduce_aq_mutant_parasite_events(node["info"], config);
  }

  if (name == "introduce_lumefantrine_mutant_parasites") {
    events = build_introduce_lumefantrine_mutant_parasite_events(node["info"], config);
  }

  if (name == "introduce_parasites") {
    events = build_introduce_parasite_events(node["info"], config);
  }
  if (name == "introduce_parasites_periodically") {
    events = build_introduce_parasites_periodically_events(node["info"], config);
  }
  if (name == "change_treatment_coverage") {
    events = build_change_treatment_coverage_event(node["info"], config);
  }

  if (name == "change_treatment_strategy") {
    events = build_change_treatment_strategy_event(node["info"], config);
  }

  if (name == "single_round_MDA") {
    events = build_single_round_mda_event(node["info"], config);
  }

  if (name == "modify_nested_mft_strategy") {
    events = build_modify_nested_mft_strategy_event(node["info"], config);
  }
  if (name == "turn_on_mutation") {
    events = build_turn_on_mutation_event(node["info"], config);
  }
  if (name == "turn_off_mutation") {
    events = build_turn_off_mutation_event(node["info"], config);
  }

  if (name == AnnualBetaUpdateEvent::EventName) {
    events = build_annual_beta_update_event(node["info"], config);
  }
  if (name == AnnualCoverageUpdateEvent::EventName) {
    events = build_annual_coverage_update_event(node["info"], config);
  }
  if (name == ImportationPeriodicallyRandomEvent::EventName) {
    events = build_importation_periodically_random_event(node["info"], config);
  }
  if (name == IntroduceMutantEvent::EventName) {
    events = build_introduce_mutant_events(node["info"], config);
  }

  return events;
}

void PopulationEventBuilder::verify_single_node(const YAML::Node& node, const std::string& name) {
  if (node.size() > 1) {
    LOG(ERROR) << "More than one sub node found for " << name;
    throw std::invalid_argument("Multiple sub nodes for single node event.");
  }
}