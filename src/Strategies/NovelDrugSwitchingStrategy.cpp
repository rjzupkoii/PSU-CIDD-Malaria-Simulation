#include "NovelDrugSwitchingStrategy.h"
#include "Model.h"
#include "Core/Random.h"
#include "Therapies/Therapy.hxx"
#include "MDC/ModelDataCollector.h"
#include "Core/Config/Config.h"
#include "Helpers/TimeHelpers.h"
#include "Core/Scheduler.h"

NovelDrugSwitchingStrategy::NovelDrugSwitchingStrategy() : IStrategy("NovelDrugSwitchingStrategy",
                                                                     NovelDrugSwitching) {}

void NovelDrugSwitchingStrategy::add_therapy(Therapy *therapy) {
  therapy_list.push_back(therapy);
}

Therapy *NovelDrugSwitchingStrategy::get_therapy(Person *person) {
  const auto p = Model::RANDOM->random_flat(0.0, 1.0);

  double sum = 0;
  for (std::size_t i = 0; i < distribution.size(); i++) {
    sum += distribution[i];
    if (p <= sum) {
      return therapy_list[i];
    }
  }

  return therapy_list[therapy_list.size() - 1];
}

std::string NovelDrugSwitchingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto *therapy : therapy_list) {
    sstm << sep << therapy->id();
    sep = ",";
  }
  sep = "";
  sstm << "-";
  for (auto dist : distribution) {
    sstm << sep << dist;
    sep = ",";
  }
  return sstm.str();
}

void NovelDrugSwitchingStrategy::update_end_of_time_step() {
  //check and switch
  if (therapy_list[1]->id()!=switch_to &&
      Model::DATA_COLLECTOR->current_tf_by_therapy()[therapy_list[0]->id()] >= tf_threshold) {
    therapy_list[1] = Model::CONFIG->therapy_db()[switch_to];

    //reset the time point to collect ntf
    Model::CONFIG->start_of_comparison_period() = Model::SCHEDULER->current_time();

    //reset the total time to 20 years after this time point
    const date::sys_days next_20_year{date::year_month_day{Model::SCHEDULER->calendar_date} + date::years{20}};
    const auto new_total_time = Model::SCHEDULER->current_time() + TimeHelpers::number_of_days(
        Model::SCHEDULER->calendar_date, next_20_year);

    if (new_total_time > Model::CONFIG->total_time()) {
      //extend the scheduler
      Model::SCHEDULER->extend_total_time(new_total_time);
    }
    Model::CONFIG->total_time() = new_total_time;
    LOG(INFO) << date::year_month_day{Model::SCHEDULER->calendar_date} << ": Switch to novel drug with id "
              << switch_to;

  }
}

void NovelDrugSwitchingStrategy::adjust_started_time_point(const int &current_time) {}

void NovelDrugSwitchingStrategy::monthly_update() {}
