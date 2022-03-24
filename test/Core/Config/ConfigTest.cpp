#include "Core/Config/Config.h"
#include "Spatial/WesolowskiSM.h"
#include "Spatial/BarabasiSM.h"
#include "Therapies/Therapy.hxx"
#include "Therapies/SCTherapy.h"
#include "Strategies/IStrategy.h"
#include "Events/Event.h"
#include "date/date.h"
#include "Events/Population/SingleRoundMDAEvent.h"
#include <catch2/catch.hpp>

TEST_CASE("ConfigTest", "[Core]") {
  Config c;
  c.read_from_file("input.yml");

  SECTION("Config can read from default input.yml") {


    REQUIRE(c.starting_date() == date::year_month_day{date::year{1990} / 1 / 1});
    REQUIRE(c.ending_date() == date::year_month_day{date::year{1991} / 1 / 1});

    REQUIRE(c.total_time() == 365);

    REQUIRE(c.start_collect_data_day() == 365);
    REQUIRE(c.start_of_comparison_period() == 10926);

    REQUIRE(c.number_of_tracking_days() == 11);
    REQUIRE(c.p_infection_from_an_infectious_bite() == 0.1);

    REQUIRE(c.age_structure() == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 20, 60, 100});
    REQUIRE(c.initial_age_structure() ==
            std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 25, 35, 45, 55, 65, 100});

    REQUIRE(c.tf_testing_day() == 28);

    REQUIRE(c.days_to_clinical_under_five() == 4);
    REQUIRE(c.days_to_clinical_over_five() == 6);
    REQUIRE(c.days_mature_gametocyte_under_five() == 4);
    REQUIRE(c.days_mature_gametocyte_over_five() == 6);

    REQUIRE(c.p_compliance() == 1.0);
    REQUIRE(c.min_dosing_days() == 1);

    REQUIRE(c.gametocyte_level_full() == 1.0);

    REQUIRE(c.p_relapse() == 0.01);
    REQUIRE(c.relapse_duration() == 30);

    REQUIRE(c.allow_new_coinfection_to_cause_symtoms() == true);
    REQUIRE(c.update_frequency() == 7);
    REQUIRE(c.report_frequency() == 30);

    REQUIRE(c.tf_rate() == 0.1);
    REQUIRE(c.using_free_recombination() == true);
    REQUIRE(c.tf_window_size() == 60);

    REQUIRE(c.using_age_dependent_bitting_level() == false);
    REQUIRE(c.using_variable_probability_infectious_bites_cause_infection() == false);

    REQUIRE(c.fraction_mosquitoes_interrupted_feeding() == 0.0);
    REQUIRE(c.inflation_factor() == 0.01);

    REQUIRE(c.location_db().size() == 9);
    REQUIRE(c.location_db()[0].id == 0);
    REQUIRE(c.location_db()[8].id == 8);
    REQUIRE(c.location_db()[0].age_distribution ==
            std::vector<double>{0.0334, 0.0300, 0.0329, 0.0324, 0.0332, 0.0314, 0.0316, 0.0310,
                                0.0285, 0.0256, 0.0298, 0.0212, 0.0321, 0.0228, 0.0230, 0.1906, 0.1403, 0.0966,
                                0.0605,
                                0.0344, 0.0387});
    REQUIRE(c.location_db()[8].age_distribution ==
            std::vector<double>{0.0338, 0.0300, 0.0329, 0.0324, 0.0332, 0.0314, 0.0316, 0.0310,
                                0.0285, 0.0256, 0.0298, 0.0212, 0.0321, 0.0228, 0.0230, 0.1906, 0.1403, 0.0966,
                                0.0605,
                                0.0344, 0.0387});

    REQUIRE(c.location_db()[0].p_treatment_less_than_5 == 0.6f);
    REQUIRE(c.location_db()[0].p_treatment_more_than_5 == 0.5f);

    REQUIRE(c.location_db()[8].p_treatment_less_than_5 == 0.6f);
    REQUIRE(c.location_db()[8].p_treatment_more_than_5 == 0.5f);

    REQUIRE(c.location_db()[0].beta == 0.05f);
    REQUIRE(c.location_db()[8].beta == 0.05f);

    REQUIRE(c.location_db()[0].population_size == 1000);
    REQUIRE(c.location_db()[8].population_size == 10090);

    REQUIRE(c.birth_rate() == 0.03725);

    REQUIRE(c.death_rate_by_age_class() ==
            DoubleVector{0.053979329, 0.018935757, 0.006867257, 0.001124347, 0.001136455, 0.001606066,
                         0.001953783, 0.001530096, 0.001299153, 0.001068073, 0.000978264, 0.000978264, 0.0055,
                         0.0055,
                         0.04450});

    REQUIRE(c.mortality_when_treatment_fail_by_age_class() ==
            DoubleVector{0.040, 0.020, 0.020, 0.020, 0.020, 0.004, 0.004, 0.004, 0.004, 0.004,
                         0.004, 0.001, 0.001, 0.001, 0.001});

    REQUIRE(c.parasite_density_level().log_parasite_density_cured == -4.699);
    REQUIRE(c.parasite_density_level().log_parasite_density_from_liver == -2.0);
    REQUIRE(c.parasite_density_level().log_parasite_density_asymptomatic == 3);
    REQUIRE(c.parasite_density_level().log_parasite_density_clinical == 4.301);
    REQUIRE(c.parasite_density_level().log_parasite_density_clinical_from == 3.301);
    REQUIRE(c.parasite_density_level().log_parasite_density_clinical_to == 5.301);
    REQUIRE(c.parasite_density_level().log_parasite_density_detectable == 1.000);
    REQUIRE(c.parasite_density_level().log_parasite_density_pyrogenic == 3.398);

    REQUIRE(c.genotype_info().loci_vector.size() == 5);

    REQUIRE(c.relative_infectivity().sigma == log(10) / 3.91);

    REQUIRE(c.number_of_age_classes() == 15);

    REQUIRE(c.number_of_locations() == 9);

    REQUIRE(c.spatial_distance_matrix().size() == 9);
    REQUIRE(c.spatial_distance_matrix()[8].size() == 9);

    REQUIRE(c.seasonal_info().A == DoubleVector(9, 1.0));

    REQUIRE(dynamic_cast<Spatial::WesolowskiSM*>(c.spatial_model()) != nullptr);
    REQUIRE(dynamic_cast<Spatial::WesolowskiSM*>(c.spatial_model())->kappa() == 0.01093251);
    REQUIRE(dynamic_cast<Spatial::BarabasiSM*>(c.spatial_model()) == nullptr);

    REQUIRE(c.immune_system_information().acquire_rate == 0.00125);
    REQUIRE(c.immune_system_information().decay_rate == 0.0025);

    REQUIRE(c.genotype_db() != nullptr);
    REQUIRE(c.genotype_db()->size() == 128);
    REQUIRE(c.genotype_db()->at(0)->gene_expression() == IntVector{0, 0, 0, 0, 0});
    REQUIRE(c.genotype_db()->at(127)->gene_expression() == IntVector{1, 7, 1, 1, 1});

    //
    REQUIRE(c.number_of_parasite_types() == 128);
    //
    REQUIRE(c.drug_db()->size() == 7);
    REQUIRE(c.drug_db()->at(0)->n() == 25);
    REQUIRE(c.drug_db()->at(6)->n() == 19);

    //
    REQUIRE(c.EC50_power_n_table().size() == 128);
    REQUIRE(c.EC50_power_n_table()[0].size() == 7);
    //
    REQUIRE(c.circulation_info().max_relative_moving_value == 35);
    //
    REQUIRE(c.relative_bitting_info().max_relative_biting_value == 35);

    REQUIRE(c.therapy_db().size() == 13);
    REQUIRE(c.therapy_db()[0]->drug_ids.size() == 1);
    REQUIRE(c.therapy_db()[0]->drug_ids == IntVector{0});
    REQUIRE(((SCTherapy*) c.therapy_db()[0])->dosing_day[0] == 3);

    REQUIRE(c.therapy_db()[11]->drug_ids.size() == 3);
    REQUIRE(c.therapy_db()[11]->drug_ids == IntVector{0, 1, 2});
    REQUIRE(((SCTherapy*) c.therapy_db()[11])->dosing_day[0] == 3);

    REQUIRE(c.strategy_db().size() == 15);
    REQUIRE(c.strategy_db()[0]->name == "SP-AQ-CQ-AL-MFTStrategy");
    REQUIRE(c.strategy_db()[0]->get_type() == IStrategy::MFT);
    REQUIRE(c.strategy_db()[14]->name == "BaseLineStrategy");
    REQUIRE(c.strategy_db()[14]->get_type() == IStrategy::NestedMFTMultiLocation);

    REQUIRE(c.initial_strategy_id() == 5);

    REQUIRE(c.initial_parasite_info().size() == 18);
    for (auto i = 0; i < 18; i += 2) {
      REQUIRE(c.initial_parasite_info()[i].parasite_type_id == 64);
      REQUIRE(c.initial_parasite_info()[i].prevalence == 0.05);
    }
    for (auto i = 1; i < 18; i += 2) {
      REQUIRE(c.initial_parasite_info()[i].parasite_type_id == 72);
      REQUIRE(c.initial_parasite_info()[i].prevalence == 0.05);
    }

    // REQUIRE(c.initial_parasite_info()[18].parasite_type_id == 15);
    // REQUIRE(c.initial_parasite_info()[18].prevalence == 0.05);
    // REQUIRE(c.initial_parasite_info()[18].location == 10);
    // REQUIRE(c.initial_parasite_info()[19].parasite_type_id == 20);
    // REQUIRE(c.initial_parasite_info()[19].prevalence == 0.1);
    // REQUIRE(c.initial_parasite_info()[19].location == 10);

    REQUIRE(c.preconfig_population_events().size() == 14);
    REQUIRE(c.preconfig_population_events()[0]->time == 78);
    REQUIRE(c.preconfig_population_events()[1]->time == 104);
    REQUIRE(c.preconfig_population_events()[2]->time == 236);
    REQUIRE(c.preconfig_population_events()[3]->time == 292);
    REQUIRE(c.preconfig_population_events()[4]->time == 152);
    REQUIRE(c.preconfig_population_events()[5]->time == 31);
    REQUIRE(c.preconfig_population_events()[6]->time == 59);
    REQUIRE(c.preconfig_population_events()[7]->time == 151);
    REQUIRE(c.preconfig_population_events()[8]->time == 60);
    REQUIRE(c.preconfig_population_events()[9]->time == 152);
    REQUIRE(c.preconfig_population_events()[10]->time == 244);
    REQUIRE(c.preconfig_population_events()[11]->time == 10957);

    REQUIRE(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[11])->fraction_population_targeted[0] ==
            1);
    REQUIRE(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[11])->fraction_population_targeted[1] ==
            1);
    REQUIRE(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[11])->fraction_population_targeted[2] ==
            1);
    REQUIRE(dynamic_cast<SingleRoundMDAEvent*>(c.preconfig_population_events()[11])->days_to_complete_all_treatments ==
            14);

    REQUIRE(c.preconfig_population_events()[12]->time == 10995);
    REQUIRE(c.preconfig_population_events()[13]->time == 11030);

    REQUIRE(c.bitting_level_generator().level_density.size() == 100);
    REQUIRE(c.moving_level_generator().level_density.size() == 100);


    // REQUIRE(2== 2);
  }

  SECTION("Config can read MDA Individual parameters") {

    REQUIRE(c.age_bracket_prob_individual_present_at_mda().size() == 2);
    REQUIRE(c.age_bracket_prob_individual_present_at_mda()[0] == 10);
    REQUIRE(c.age_bracket_prob_individual_present_at_mda()[1] == 40);

    REQUIRE(c.mean_prob_individual_present_at_mda().size() == 3);
    REQUIRE(c.mean_prob_individual_present_at_mda()[0] == 0.85);
    REQUIRE(c.mean_prob_individual_present_at_mda()[1] == 0.75);
    REQUIRE(c.mean_prob_individual_present_at_mda()[2] == 0.85);

    REQUIRE(c.sd_prob_individual_present_at_mda().size() == 3);
    REQUIRE(c.sd_prob_individual_present_at_mda()[0] == 0.3);
    REQUIRE(c.sd_prob_individual_present_at_mda()[1] == 0.3);
    REQUIRE(c.sd_prob_individual_present_at_mda()[2] == 0.3);
  }

  SECTION("Config has MDA prob_individual_present_at_mda_distribution") {
    REQUIRE(c.prob_individual_present_at_mda_distribution().size() == 3);
    std::cout << c.prob_individual_present_at_mda_distribution()[0].alpha << " - "
              << c.prob_individual_present_at_mda_distribution()[0].
                  beta
              << std::endl;
    std::cout << c.prob_individual_present_at_mda_distribution()[1].alpha << " - "
              << c.prob_individual_present_at_mda_distribution()[1].
                  beta
              << std::endl;
    std::cout << c.prob_individual_present_at_mda_distribution()[2].alpha << " - "
              << c.prob_individual_present_at_mda_distribution()[2].
                  beta
              << std::endl;
  }
}
