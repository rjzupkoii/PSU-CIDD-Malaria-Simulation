#include "MonthlyReporterMosquitoes.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Core/Random.h"
#include "Strategies/IStrategy.h"
#include "Helpers/TimeHelpers.h"
#include "Constants.h"
#include "easylogging++.h"
#include <date/date.h>
#include "Population/Population.h"
#include "Reporters/ReporterUtils.h"
#include "Population/Properties/PersonIndexAll.h"
#include <math.h>
#include <filesystem>

MonthlyReporterMosquitoes::MonthlyReporterMosquitoes() = default;

MonthlyReporterMosquitoes::~MonthlyReporterMosquitoes() = default;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
inline const std::string getCurrentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M", &tstruct);

    return buf;
}

void MonthlyReporterMosquitoes::initialize(int job_number, std::string path) {
    // Create the configuration for the monthly reporter
    el::Configurations monthly_reporter_custom_logger;
    monthly_reporter_custom_logger.setToDefault();
    monthly_reporter_custom_logger.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
    monthly_reporter_custom_logger.setGlobally(el::ConfigurationType::ToFile, "true");

    ss.str("");
    std::string reporter_output_path = "Monthly_Data_";

    /* DEBUG
    // Update the study id if one was provided
    if (!Model::MODEL->study_tag().empty()) {
        std::string study_tag = Model::MODEL->study_tag();
        LOG(INFO) << "Associating study tag " << study_tag << " with configuration.";
        ss << reporter_output_path << study_tag << "_" ;
        reporter_output_path = ss.str();
        ss.str("");
    }
    */
    ss << getCurrentDateTime() << "_" << reporter_output_path;
    reporter_output_path  = ss.str();
    ss.str("");
    VLOG(0) << "Report output path: " << path << reporter_output_path << job_number << ".txt";
    monthly_reporter_custom_logger.setGlobally(el::ConfigurationType::Filename, fmt::format("{}"+ reporter_output_path +"{}.txt", path, job_number));
    monthly_reporter_custom_logger.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    monthly_reporter_custom_logger.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
    el::Loggers::reconfigureLogger("monthly_reporter_custom", monthly_reporter_custom_logger);

    auto const& prmc = Model::CONFIG->prmc();

    // Log the aggregate headers
        ss
        << "Day" << Csv::sep
        << "Calendar" << Csv::sep
        << "ArtResistanceFrequencyAt15" << Csv::sep
        << "CurrentNumberOfMutationEvents" << Csv::sep
        << "CurrentUTlDuration" << Csv::sep
        << "DiscountedAFU" << Csv::sep
        << "DiscountedAMUForClinicalCausedParasite" << Csv::sep
        << "DiscountedAMUPerParasitePopulation" << Csv::sep
        << "DiscountedAMUPerPerson" << Csv::sep
        << "DoubleResistanceFrequencyAt15" << Csv::sep
        << "MeanMOI" << Csv::sep
        << "QuadrupleResistanceFrequencyAt15" << Csv::sep
        << "QuintupleResistanceFrequencyAt15" << Csv::sep
        << "SingleResistanceFrequencyAt15" << Csv::sep
        << "TFAt15" << Csv::sep
        << "TotalResistanceFrequencyAt15" << Csv::sep
       ;
    for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        ss
        << location << ":" << "BloodSlidePrevalence" << Csv::sep
        << location << ":" << "Births" << Csv::sep
        << location << ":" << "BloodSlideNumberLT5" << Csv::sep
        << location << ":" << "BloodSlideNumberMT5" << Csv::sep
        << location << ":" << "CurrentEIR" << Csv::sep
        << location << ":" << "CurrentTF" << Csv::sep
        << location << ":" << "CurrentRITF" << Csv::sep
        << location << ":" << "CumulativeNTF" << Csv::sep
        << location << ":" << "CumulativeClinicalEpisodes" << Csv::sep
        << location << ":" << "CumulativeDiscountedNTF" << Csv::sep
        << location << ":" << "CumulativeMutants" << Csv::sep
        << location << ":" << "Deaths" << Csv::sep
        << location << ":" << "FractionOfPositiveThatAreClinical" << Csv::sep
        << location << ":" << "MalariaDeaths" << Csv::sep
        << location << ":" << "MalariaDeathsLT5" << Csv::sep
        << location << ":" << "MalariaDeathsMT5" << Csv::sep
        << location << ":" << "MonthlyNumberOfClinicalEpisode" << Csv::sep
        << location << ":" << "MonthlyNumberOfNewInfections" << Csv::sep
        << location << ":" << "MonthlyNumberOfTreatment" << Csv::sep
        << location << ":" << "MonthlyNumberOfTreatmentFailure" << Csv::sep
        << location << ":" << "MonthlyNumberOfNonTreatment" << Csv::sep
        << location << ":" << "NumberOfClinicalLT5" << Csv::sep
        << location << ":" << "NumberOfClinicalMT5" << Csv::sep
        << location << ":" << "NumberOfDeathLT5" << Csv::sep
        << location << ":" << "NumberOFDeathMT5" << Csv::sep
        << location << ":" << "NumberOfPositive" << Csv::sep
        << location << ":" << "PercentageBitesOnTop20" << Csv::sep
        << location << ":" << "PopulationSize" << Csv::sep
        << location << ":" << "PopulationSizeResidence" << Csv::sep
        << location << ":" << "TodayRITF" << Csv::sep
        << location << ":" << "TodayTF" << Csv::sep
        << location << ":" << "TodayNumberOfTreatments" << Csv::sep
        << location << ":" << "TotalNumberOfBites" << Csv::sep
        << location << ":" << "TotalImmune" << Csv::sep
        << location << ":" << "TotalImmuneLT5" << Csv::sep
        << location << ":" << "TotalImmuneMT5" << Csv::sep
        << location << ":" << "TotalParasitePopulation" << Csv::sep
        << location << ":" << "TotalParasitePopulationLT5" << Csv::sep
        << location << ":" << "TotalParasitePopulationMT5" << Csv::sep
        << location << ":" << "ProbabilityToBeTreatedLT5" << Csv::sep
        << location << ":" << "ProbabilityToBeTreatedMT5" << Csv::sep
        << location << ":" << "EIR" << Csv::sep
        << location << ":" << "EIRLog10" << Csv::sep
        << location << ":" << "PRMCCurrentInterruptedFeedingRate" << Csv::sep;
        prmc->printRecombinedGenotypeHeaders(ss);
        ss << Csv::sep;
        prmc->printPopulationGenotypeHeaders(ss);
        if(location < Model::CONFIG->number_of_locations() - 1){
            ss << Csv::sep;
        }
    }
    ss << Csv::end_line;
    CLOG(INFO, "monthly_reporter_custom") << ss.str();
    ss.str("");
}

void MonthlyReporterMosquitoes::monthly_report()
{
    auto const& prmc = Model::CONFIG->prmc();

    ss
    << Model::SCHEDULER->current_time() << Csv::sep
    << date::format("%Y", Model::SCHEDULER->calendar_date) << Csv::sep
    << Model::DATA_COLLECTOR->art_resistance_frequency_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->current_number_of_mutation_events() << Csv::sep
    << Model::DATA_COLLECTOR->current_utl_duration() << Csv::sep
    << Model::DATA_COLLECTOR->discounted_AFU() << Csv::sep
    << Model::DATA_COLLECTOR->discounted_AMU_for_clinical_caused_parasite() << Csv::sep
    << Model::DATA_COLLECTOR->discounted_AMU_per_parasite_pop() << Csv::sep
    << Model::DATA_COLLECTOR->discounted_AMU_per_person() << Csv::sep
    << Model::DATA_COLLECTOR->double_resistance_frequency_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->mean_moi() << Csv::sep
    << Model::DATA_COLLECTOR->quadruple_resistance_frequency_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->quintuple_resistance_frequency_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->single_resistance_frequency_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->tf_at_15() << Csv::sep
    << Model::DATA_COLLECTOR->total_resistance_frequency_at_15() << Csv::sep
    ;
    for (unsigned int location = 0; location < Model::CONFIG->number_of_locations(); location++) {
        auto EIR = Model::DATA_COLLECTOR->EIR_by_location_year()[location].empty()
                ? 0 : Model::DATA_COLLECTOR->EIR_by_location_year()[location].back();
        auto EIRLog10 = EIR == 0 ? 0 : log10(EIR);

        ss
        << Model::DATA_COLLECTOR->blood_slide_prevalence_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->births_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->blood_slide_number_by_location_age_group()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->blood_slide_number_by_location_age_group()[location][10] << Csv::sep
        << Model::DATA_COLLECTOR->current_EIR_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->current_TF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->current_RITF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->cumulative_NTF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->cumulative_clinical_episodes_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->cumulative_discounted_NTF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->cumulative_mutants_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->deaths_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->fraction_of_positive_that_are_clinical_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->malaria_deaths_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->malaria_deaths_by_location_age_class()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->malaria_deaths_by_location_age_class()[location][10] << Csv::sep
        << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->monthly_number_of_new_infections_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->monthly_treatment_failure_by_location()[location]<< Csv::sep
        << Model::DATA_COLLECTOR->monthly_nontreatment_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->number_of_clinical_by_location_age_group()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->number_of_clinical_by_location_age_group()[location][10] << Csv::sep
        << Model::DATA_COLLECTOR->number_of_death_by_location_age_group()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->number_of_death_by_location_age_group()[location][10] << Csv::sep
        << Model::DATA_COLLECTOR->number_of_positive_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->percentage_bites_on_top_20_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->popsize_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->popsize_residence_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->today_RITF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->today_TF_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->today_number_of_treatments_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->total_number_of_bites_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->total_immune_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->total_immune_by_location_age_class()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->total_immune_by_location_age_class()[location][10] << Csv::sep
        << Model::DATA_COLLECTOR->total_parasite_population_by_location()[location] << Csv::sep
        << Model::DATA_COLLECTOR->total_parasite_population_by_location_age_group()[location][1] << Csv::sep
        << Model::DATA_COLLECTOR->total_parasite_population_by_location_age_group()[location][10] << Csv::sep
        << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(location, 1) << Csv::sep
        << Model::TREATMENT_COVERAGE->get_probability_to_be_treated(location, 10) << Csv::sep
        << EIR << Csv::sep
        << EIRLog10 << Csv::sep;
        prmc->printRecombinedGenotypeStats(ss);
        ss << Csv::sep;
        prmc->printPopulationGenotypeStats(ss);
        if(location < Model::CONFIG->number_of_locations() - 1){
            ss << Csv::sep;
        }
    }
    CLOG(INFO, "monthly_reporter_custom") << ss.str();
    ss.str("");
}

void MonthlyReporterMosquitoes::before_run(){
}