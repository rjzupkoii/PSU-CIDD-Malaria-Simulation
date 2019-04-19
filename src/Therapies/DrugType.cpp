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

#ifndef LOG2_10
#define LOG2_10 3.32192809489
#endif

DrugType::DrugType() : id_(0), drug_half_life_(0), maximum_parasite_killing_rate_(0),
                       p_mutation_(0), k_(0), cut_off_percent_(0), n_(1) {}

DrugType::~DrugType() = default;

void DrugType::reset(int length) {
  //    gene_expression_.resize(length);
  //    gene_expression_.reset();
}

//void DrugType::set_resistance_position(int pos) {

//
//    gene_expression_.set(pos);
//    mutation_positions_.push_back(pos);
//    //    id_ = gene_expression_.to_ulong();
//
//}
//
//double DrugType::get_parasite_killing_rate_by_concentration(const double& concentration) {
//    double conPowerN = pow(concentration, n_);
//    return maximum_parasite_killing_rate_ * conPowerN / (conPowerN + EC50_power_n_);
//}

double DrugType::get_parasite_killing_rate_by_concentration(const double &concentration, const double &EC50_power_n) {
  const auto con_power_n = pow(concentration, n_);
  return maximum_parasite_killing_rate_ * con_power_n / (con_power_n + EC50_power_n);
}

double DrugType::n() {
  return n_;
}

void DrugType::set_n(const double &n) {
  n_ = n;
  //    set_EC50_power_n(pow(EC50_, n_));
}

//
//double DrugType::EC50() {
//    return EC50_;
//}

//void DrugType::set_EC50(const double& EC50) {
//    EC50_ = EC50;
//    set_EC50_power_n(pow(EC50_, n_));
//}

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
    auto i = 0;
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
  //
  //    for (const auto& item : ec50_map_) {
  //        // if match then return ec50
  //
  //    }

  assert(false);
  //hopefully it will never reach here
  return 0;

  //
  //
  //
  //
  //    double minEC50 = is_artemisinin() ? 0.6 : 0.6;
  //    double maxEC50 = is_artemisinin() ? 1.25 : 1.6;
  //    double EC50 = minEC50;
  //
  //    for (int i = 0; i < affecting_loci_.size(); i++) {
  //        auto locus = affecting_loci_[i];
  //        for (int j = 0; j < selecting_alleles_[i].size(); j++) {
  //            auto selected_allele = selecting_alleles_[i][j];
  //            if (genotype->gene_expression()[locus] == selected_allele) {
  //                EC50 += resistant_factor_[i][j]*(maxEC50 - minEC50) / affecting_loci_.size();
  //                break;
  //            }
  //        }
  //    }
  //    return EC50;
}
