/* 
 * File:   DrugType.cpp
 * Author: nguyentran
 * 
 * Created on June 3, 2013, 10:59 AM
 */

#include "DrugType.h"
#include <cmath>
#include "Parasites/Genotype.h"
#include "Model.h"
#include "Core/Config/Config.h"
#include "Core/Random.h"

#include "easylogging++.h"

#ifndef LOG2_10
#define LOG2_10 3.32192809489
#endif

DrugType::DrugType() : id_(0), drug_half_life_(0), maximum_parasite_killing_rate_(0),
                       p_mutation_(0), k_(0), cut_off_percent_(0), n_(1) {}

double DrugType::get_parasite_killing_rate_by_concentration(const double &concentration, const double &EC50_power_n) {
  const auto con_power_n = pow(concentration, n_);
  auto result = maximum_parasite_killing_rate_ * con_power_n / (con_power_n + EC50_power_n);
  return result;
}

int DrugType::get_total_duration_of_drug_activity(const int &dosing_days) const {
  //CutOffPercent is 10
  //log2(100.0 / 10.0) = 3.32192809489
  return dosing_days + ceil(drug_half_life_ * LOG2_10);
}

int DrugType::select_mutation_locus() {
  const int pos = Model::RANDOM->random_uniform_int(0, affecting_loci_.size());
  return affecting_loci_[pos];
}

double DrugType::infer_ec50(Genotype* genotype) {
  //compare genotype.short_name with Ccnfig to get EC50
  for (auto &item : ec50_map_) {
    auto key = item.first;
    std::size_t i = 0;
    for (i = 0; i < genotype->gene_expression().size(); i++) {
      if (key[i] == '.')
        continue;
      if ((key[i] - '0') == genotype->gene_expression()[i])
        continue;
      break;
    }

    if (i == genotype->gene_expression().size()) {
      ///found match
      return item.second;
    }
  }
  
  assert(false);
  //hopefully it will never reach here
  return 0;
}
