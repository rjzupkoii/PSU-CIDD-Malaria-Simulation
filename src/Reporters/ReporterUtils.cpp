//
// Created by Nguyen Tran on 2019-04-18.
//

#include <vector>
#include <Core/Config/Config.h>
#include "ReporterUtils.h"
#include "Population/Properties/PersonIndexByLocationStateAgeClass.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Population/ClonalParasitePopulation.h"

const std::string group_sep = "-1111\t";
const std::string sep = "\t";


void ReporterUtils::output_genotype_frequency1(std::stringstream &ss, const int &number_of_genotypes,
                                               PersonIndexByLocationStateAgeClass* pi) {
  auto sum1_all = 0.0;
  std::vector<double> result1_all(number_of_genotypes, 0.0);

  const auto number_of_locations = pi->vPerson().size();
  const auto number_of_age_classes = pi->vPerson()[0][0].size();

  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    std::vector<double> result1(number_of_genotypes, 0.0);
    auto sum1 = 0.0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (std::size_t ac = 0; ac < number_of_age_classes; ac++) {
        const auto size = pi->vPerson()[loc][hs][ac].size();
        for (auto i = 0ull; i < size; i++) {
          auto* person = pi->vPerson()[loc][hs][ac][i];

          if (!person->all_clonal_parasite_populations()->parasites()->empty()) {
            sum1 += 1;
            sum1_all += 1;
          }
          // sum2 += person->all_clonal_parasite_populations()->parasites()->size();
          std::map<int, int> individual_genotype_map;

          for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
            const auto g_id = parasite_population->genotype()->genotype_id();
            // result2[g_id] += 1;
            if (individual_genotype_map.find(g_id) == individual_genotype_map.end()) {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] = 1;
            } else {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] += 1;
            }
          }

          for (const auto genotype : individual_genotype_map) {
            result1[genotype.first] += 1;
            result1_all[genotype.first] += 1;
          }
        }
      }
    }

    for (auto &i : result1) {
      i /= sum1;
    }

    for (auto &i : result1) {
      ss << i << sep;
    }
  }
  ss << group_sep;
  for (auto &i : result1_all) {
    i /= sum1_all;
  }
  for (auto &i : result1_all) {
    ss << i << sep;
  }
}

void ReporterUtils::output_genotype_frequency2(std::stringstream &ss, const int &number_of_genotypes,
                                               PersonIndexByLocationStateAgeClass* pi) {
  auto sum2_all = 0.0;
  std::vector<double> result2_all(number_of_genotypes, 0.0);
  const auto number_of_locations = pi->vPerson().size();
  const auto number_of_age_classes = pi->vPerson()[0][0].size();

  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    std::vector<double> result2(number_of_genotypes, 0.0);
    auto sum2 = 0.0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (std::size_t ac = 0; ac < number_of_age_classes; ac++) {
        const auto size = pi->vPerson()[loc][hs][ac].size();
        for (auto i = 0ull; i < size; i++) {
          auto* person = pi->vPerson()[loc][hs][ac][i];

          sum2 += person->all_clonal_parasite_populations()->parasites()->size();
          sum2_all += sum2;
          std::map<int, int> individual_genotype_map;

          for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
            const auto g_id = parasite_population->genotype()->genotype_id();
            result2[g_id] += 1;
            result2_all[g_id] += 1;
          }
        }
      }
    }

    // output for each location

    for (auto &i : result2) {
      i /= sum2;
    }
    for (auto &i : result2) {
      ss << i << sep;
    }

  }
  // output for all locations
  ss << group_sep;
  for (auto &i : result2_all) {
    i /= sum2_all;
  }
  for (auto &i : result2_all) {
    ss << i << sep;
  }
}

void ReporterUtils::output_genotype_frequency3(std::stringstream &ss, const int &number_of_genotypes,
                                               PersonIndexByLocationStateAgeClass* pi) {
  auto sum1_all = 0.0;
  std::vector<double> result3_all(number_of_genotypes, 0.0);
  const auto number_of_locations = pi->vPerson().size();
  const auto number_of_age_classes = pi->vPerson()[0][0].size();

  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    std::vector<double> result3(number_of_genotypes, 0.0);
    auto sum1 = 0.0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (std::size_t ac = 0; ac < number_of_age_classes; ac++) {
        const auto size = pi->vPerson()[loc][hs][ac].size();
        for (auto i = 0ull; i < size; i++) {
          auto* person = pi->vPerson()[loc][hs][ac][i];

          if (!person->all_clonal_parasite_populations()->parasites()->empty()) {
            sum1 += 1;
            sum1_all += 1;
          }

          std::map<int, int> individual_genotype_map;

          for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
            const auto g_id = parasite_population->genotype()->genotype_id();
            if (individual_genotype_map.find(g_id) == individual_genotype_map.end()) {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] = 1;
            } else {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] += 1;
            }
          }

          for (const auto genotype : individual_genotype_map) {
            result3[genotype.first] += genotype.second /
                                       static_cast<double>(person->all_clonal_parasite_populations()->parasites()->size()
                                       );
            result3_all[genotype.first] += genotype.second / static_cast<double>(person
                ->all_clonal_parasite_populations()->parasites()->size());
          }
        }
      }
    }
    for (auto &i : result3) {
      i /= sum1;
    }

    //this is for per location
    for (auto &i : result3) {
      ss << i << sep;
    }
  }

  //this is for all locations
  ss << group_sep;
  for (auto &i : result3_all) {
    i /= sum1_all;
  }
  for (auto &i : result3_all) {
    ss << i << sep;
  }

  ss << group_sep;
  ss << sum1_all << sep;
}

void ReporterUtils::output_3_genotype_frequency(std::stringstream &ss, const int &number_of_genotypes,
                                                PersonIndexByLocationStateAgeClass* pi) {
  auto sum1_all = 0.0;
  auto sum2_all = 0.0;
  std::vector<double> result1_all(number_of_genotypes, 0.0);
  std::vector<double> result2_all(number_of_genotypes, 0.0);
  std::vector<double> result3_all(number_of_genotypes, 0.0);

  const auto number_of_locations = pi->vPerson().size();
  const auto number_of_age_classes = pi->vPerson()[0][0].size();

  for (std::size_t loc = 0; loc < number_of_locations; loc++) {
    std::vector<double> result1(number_of_genotypes, 0.0);
    std::vector<double> result2(number_of_genotypes, 0.0);
    std::vector<double> result3(number_of_genotypes, 0.0);

    auto sum2 = 0.0;
    auto sum1 = 0.0;

    for (auto hs = 0; hs < Person::NUMBER_OF_STATE - 1; hs++) {
      for (std::size_t ac = 0; ac < number_of_age_classes; ac++) {
        const auto size = pi->vPerson()[loc][hs][ac].size();
        for (auto i = 0ull; i < size; i++) {
          auto* person = pi->vPerson()[loc][hs][ac][i];

          if (!person->all_clonal_parasite_populations()->parasites()->empty()) {
            sum1 += 1;
            sum1_all += 1;
          }
          sum2 += person->all_clonal_parasite_populations()->parasites()->size();
          sum2_all += sum2;
          std::map<int, int> individual_genotype_map;

          for (auto* parasite_population : *(person->all_clonal_parasite_populations()->parasites())) {
            const auto g_id = parasite_population->genotype()->genotype_id();
            result2[g_id] += 1;
            result2_all[g_id] += 1;
            if (individual_genotype_map.find(g_id) == individual_genotype_map.end()) {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] = 1;
            } else {
              individual_genotype_map[parasite_population->genotype()->genotype_id()] += 1;
            }
          }

          for (const auto genotype : individual_genotype_map) {
            result1[genotype.first] += 1;
            result1_all[genotype.first] += 1;
            result3[genotype.first] += genotype.second /
                                       static_cast<double>(person->all_clonal_parasite_populations()->parasites()->size()
                                       );
            result3_all[genotype.first] += genotype.second / static_cast<double>(person
                ->all_clonal_parasite_populations()->parasites()->size());
          }
        }
      }
    }

    for (auto &i : result1) {
      i /= sum1;
    }

    for (auto &i : result2) {
      i /= sum2;
    }

    for (auto &i : result3) {
      i /= sum1;
    }

    for (auto j = 0; j < number_of_genotypes; ++j) {
      ss << result1[j] << sep;
      ss << result2[j] << sep;
      ss << result3[j] << sep;
    }

    ss << group_sep;
  }

  //this is for all locations
  ss << group_sep;
  for (auto j = 0; j < number_of_genotypes; ++j) {
    result1_all[j] /= sum1_all;
    result2_all[j] /= sum2_all;
    result3_all[j] /= sum1_all;

    ss << result1_all[j] << sep;
    ss << result1_all[j] << sep;
    ss << result1_all[j] << sep;
  }
}

//
//
//
//void MonthlyReporter::print_monthly_incidence_by_location() {
//  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
//    ss << Model::DATA_COLLECTOR->monthly_number_of_treatment_by_location()[loc] << sep;
//  }
//
//  ss << group_sep;
//
//  for (auto loc = 0; loc < Model::CONFIG->number_of_locations(); ++loc) {
//    ss << Model::DATA_COLLECTOR->monthly_number_of_clinical_episode_by_location()[loc] << sep;
//  }
//}

