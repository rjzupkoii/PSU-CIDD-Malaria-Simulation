/*
 * YamlConverter.hxx
 * 
 * This file defines several converters that are used for parsing the YAML configuration file.
 *
 * Due to how the YAML is being parsed, Clang-tidy returns a false positive for unused structures and functions, so we
 * have them disabled at the file level. Likewise, some parameters are required to comply with interfaces despite not
 * being used in the functions.
 */ 
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedParameter"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef YAML_CONVERTER_HXX
#define YAML_CONVERTER_HXX

#include <cmath>
#include <date/date.h>
#include <yaml-cpp/yaml.h>

#include "Core/TypeDef.h"
#include "easylogging++.h"
#include "GIS/SpatialData.h"
#include "Spatial/Location.h"

namespace YAML {
  template<>
  struct convert<date::sys_days> {
    static Node encode(const date::sys_days &rhs) {
      Node node;
      node.push_back(date::format("%Y/%m/%d", rhs));
      return node;
    }

    static bool decode(const Node &node, date::sys_days &rhs) {

      if (!node.IsScalar()) {
        return false;
      }
      std::stringstream ss(node.as<std::string>());
      date::from_stream(ss, "%Y/%m/%d", rhs);
      return true;
    }
  };

  template<>
  struct convert<date::year_month_day> {
    static Node encode(const date::year_month_day &rhs) {
      Node node;
      node.push_back(date::format("%Y/%m/%d", rhs));
      return node;
    }

    static bool decode(const Node &node, date::year_month_day &rhs) {

      if (!node.IsScalar()) {
        return false;
      }
      std::stringstream ss(node.as<std::string>());
      from_stream(ss, "%Y/%m/%d", rhs);
      return true;
    }
  };
  
  template<>
  struct convert<RasterDb> {
    static Node encode(const RasterDb &rdb) {
      Node node;
      node.push_back("raster_db");
      return node;
    }
    static bool decode(const Node &node, RasterDb &rdb) {
      auto result = SpatialData::get_instance().parse(node);
      SpatialData::get_instance().generate_distances();
      return result;
    }
  };

  template<>
  struct convert<ParasiteDensityLevel> {
    static Node encode(const ParasiteDensityLevel &rhs) {
      Node node;
      node.push_back("ParasiteDensityLevel");
      return node;
    }

    static bool decode(const Node &node, ParasiteDensityLevel &parasite_density_level) {
      parasite_density_level.log_parasite_density_cured = node["log_parasite_density_cured"].as<double>();
      parasite_density_level.log_parasite_density_from_liver = node["log_parasite_density_from_liver"].as<double>();
      parasite_density_level.log_parasite_density_asymptomatic = node["log_parasite_density_asymptomatic"].as<double>();
      parasite_density_level.log_parasite_density_clinical = node["log_parasite_density_clinical"].as<double>();
      parasite_density_level.log_parasite_density_clinical_from = node["log_parasite_density_clinical_from"].as<double>();
      parasite_density_level.log_parasite_density_clinical_to = node["log_parasite_density_clinical_to"].as<double>();
      parasite_density_level.log_parasite_density_detectable = node["log_parasite_density_detectable"].as<double>();

      // log_parasite_density_detectable_pfpr is preferred and used by the simulation; however, log_parasite_density_detectable
      // is still supported since the DxG Generator uses the value.
      parasite_density_level.log_parasite_density_detectable_pfpr = node["log_parasite_density_detectable_pfpr"]
                                                                    ? node["log_parasite_density_detectable_pfpr"].as<double>()
                                                                    : node["log_parasite_density_detectable"].as<double>();

      parasite_density_level.log_parasite_density_pyrogenic = node["log_parasite_density_pyrogenic"].as<double>();
      return true;
    }
  };

  template<>
  struct convert<GenotypeInfo> {
    static Node encode(const GenotypeInfo &rhs) {
      Node node;
      node.push_back("GenotypeInfo");
      return node;
    }

    static bool decode(const Node &node, GenotypeInfo &genotype_info) {
      genotype_info.loci_vector.clear();
      for (std::size_t i = 0; i < node["loci"].size(); i++) {
        Locus l;
        l.position = node["loci"][i]["position"].as<int>();

        for (std::size_t j = 0; j < node["loci"][i]["alleles"].size(); j++) {
          Allele al;
          al.value = node["loci"][i]["alleles"][j]["value"].as<int>();
          al.name = node["loci"][i]["alleles"][j]["allele_name"].as<std::string>();
          al.short_name = node["loci"][i]["alleles"][j]["short_name"].as<std::string>();
          al.mutation_level = node["loci"][i]["alleles"][j]["mutation_level"].as<int>();
          al.daily_cost_of_resistance = node["loci"][i]["alleles"][j]["daily_cost_of_resistance"].as<
            double>();
          for (std::size_t c = 0; c < node["loci"][i]["alleles"][j]["can_mutate_to"].size(); c++) {
            //                al.mutation_value_up.push_back(node["loci"][i]["alleles"][j]["mutation_up"][c].as<int>());
            al.mutation_values.push_back(
              node["loci"][i]["alleles"][j]["can_mutate_to"][c].as<int>());
          }

          l.alleles.push_back(al);
        }

        genotype_info.loci_vector.push_back(l);
      }
      return true;
    }
  };

  template<>
  struct convert<RelativeInfectivity> {
    static Node encode(const RelativeInfectivity &rhs) {
      Node node;
      node.push_back("RelativeInfectivity");
      return node;
    }

    static bool decode(const Node &node, RelativeInfectivity &relative_infectivity) {
      relative_infectivity.sigma = node["sigma"].as<double>();
      const auto ro = node["ro"].as<double>();
      const auto blood_meal_volume = node["blood_meal_volume"].as<double>();

      const auto d_star = 1 / blood_meal_volume;

      relative_infectivity.ro_star = (log(ro) - log(d_star)) / relative_infectivity.sigma;

      relative_infectivity.sigma = log(10) / relative_infectivity.sigma;
      return true;
    }
  };
}
#endif

#pragma clang diagnostic pop