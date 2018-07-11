#ifndef YAMLCONVERTER_H
#define YAMLCONVERTER_H
#include <date/date.h>
#include <yaml-cpp/yaml.h>
#include "Spatial/Location.h"
#include "Core/TypeDef.h"

namespace YAML {
  template <>
  struct convert<date::sys_days> {
    static Node encode(const date::sys_days& rhs) {
      Node node;
      node.push_back(date::format("%Y/%m/%d", rhs));
      return node;
    }

    static bool decode(const Node& node, date::sys_days& rhs) {

      if (!node.IsScalar()) {
        return false;
      }
      std::stringstream ss(node.as<std::string>());
      date::from_stream(ss, "%Y/%m/%d", rhs);
      return true;
    }
  };

  template <>
  struct convert<date::year_month_day> {
    static Node encode(const date::year_month_day& rhs) {
      Node node;
      node.push_back(format("%Y/%m/%d", rhs));
      return node;
    }

    static bool decode(const Node& node, date::year_month_day& rhs) {

      if (!node.IsScalar()) {
        return false;
      }
      std::stringstream ss(node.as<std::string>());
      from_stream(ss, "%Y/%m/%d", rhs);
      return true;
    }
  };


  template <>
  struct convert<std::vector<Spatial::Location>> {
    static Node encode(const std::vector<Spatial::Location>& rhs) {
      Node node;
      node.push_back("location_db");
      return node;
    }

    static bool decode(const Node& node, std::vector<Spatial::Location>& location_db) {
      //
      // if (!node.IsScalar()) {
      //   return false;
      // }
      const auto number_of_locations = node["location_info"].size();
      for (std::size_t i = 0; i < number_of_locations; i++) {
        location_db.emplace_back(node["location_info"][i][0].as<int>(),
                                 node["location_info"][i][1].as<float>(),
                                 node["location_info"][i][2].as<float>(), 0);
      }
      for (std::size_t loc = 0; loc < number_of_locations; loc++) {
        auto input_loc = node["age_distribution_by_location"].size() < number_of_locations ? 0 : loc;

        for (std::size_t i = 0; i < node["age_distribution_by_location"][input_loc].size(); i++) {
          location_db[loc].age_distribution.push_back(
            node["age_distribution_by_location"][input_loc][i].as<double>());
        }
      }
      for (std::size_t loc = 0; loc < number_of_locations; loc++) {
        auto input_loc = node["p_treatment_for_less_than_5_by_location"].size() < number_of_locations ? 0 : loc;
        location_db[loc].p_treatment_less_than_5 = node["p_treatment_for_less_than_5_by_location"][input_loc].as<float>();
      }
      for (std::size_t loc = 0; loc < number_of_locations; loc++) {
        auto input_loc = node["p_treatment_for_more_than_5_by_location"].size() < number_of_locations ? 0 : loc;
        location_db[loc].p_treatment_more_than_5 = node["p_treatment_for_more_than_5_by_location"][input_loc].as<float>();
      }
      for (std::size_t loc = 0; loc < number_of_locations; loc++) {
        auto input_loc = node["beta_by_location"].size() < number_of_locations ? 0 : loc;
        location_db[loc].beta = node["beta_by_location"][input_loc].as<float>();
      }
      for (std::size_t loc = 0; loc < number_of_locations; loc++) {
        auto input_loc = node["population_size_by_location"].size() < number_of_locations ? 0 : loc;
        location_db[loc].population_size = node["population_size_by_location"][input_loc].as<int>();
      }
      return true;
    }
  };


}
#endif // YAMLCONVERTER_H
