/*
 * CustomConfigItem.h
 * 
 * Define the various custom configuration items in the YAML file.
 * 
 * NOTE circulate dependencies will arise if the class defintions are pulled 
 *      out of this file. However, to keep things organized some of the 
 *      implimeation takes place in [classname].cpp files.  
 */
#ifndef CUSTOMCONFIGITEM_H
#define CUSTOMCONFIGITEM_H

#include <string>
#include <utility>

#include "ConfigItem.hxx"
#include "Therapies/DrugDatabase.h"
#include "Parasites/GenotypeDatabase.h"
#include "Core/MultinomialDistributionGenerator.h"

namespace YAML {
class Node;
}

class Config;

class total_time : public ConfigItem<int> {
 public:
  total_time(const std::string &name, const int &default_value, Config *config) : ConfigItem<int>(name, default_value,
                                                                                                  config) {}

  void set_value(const YAML::Node &node) override;
};

class number_of_age_classes : public ConfigItem<unsigned long> {
 public:
  number_of_age_classes(const std::string &name, const unsigned long &default_value, Config *config) : ConfigItem<unsigned long>(name,
                                                                                                             default_value,
                                                                                                             config) {}

  void set_value(const YAML::Node &node) override;
};

class number_of_locations : public ConfigItem<unsigned long> {
 public:
  number_of_locations(const std::string &name, const unsigned long &default_value, Config *config) : ConfigItem<unsigned long>(name,
                                                                                                           default_value,
                                                                                                           config) {}
  // Update the number of locations based upon the location_db size
  void set_value();

  // Update the number of locations based upon the location_db size, the node is ignored
  void set_value(const YAML::Node &node) override;
};

class spatial_distance_matrix : public ConfigItem<std::vector<std::vector<double>>> {
 public:
  spatial_distance_matrix(const std::string &name, const std::vector<std::vector<double>> &default_value,
                          Config *config) : ConfigItem<std::
                                                       vector<
      std::vector<double>>>(
      name, default_value, config) {}

  void set_value(const YAML::Node &node) override;
};

// This class defines how seasonality works in the model
class seasonal_info : public IConfigItem {
  DISALLOW_COPY_AND_ASSIGN(seasonal_info)
  DISALLOW_MOVE(seasonal_info)

  public:
    SeasonalInfo value_{};
    explicit seasonal_info(const std::string &name, SeasonalInfo default_value, Config *config = nullptr) 
      : IConfigItem(config, name), value_{ std::move(default_value) } {}
    virtual ~seasonal_info() = default;

    // Return the seasonal information
    virtual SeasonalInfo &operator()() { return value_; }

    // Set the values from the configuration file
    void set_value(const YAML::Node &node) override;

    // Return the seasonal factor for the given day and location, based upon the loaded configuration.
    static double get_seasonal_factor(const date::sys_days &today, const int &location);

  private:
    // Clear the current seasonal info
    void clear();

    // Set the values based upon the contents of a raster file.
    void set_from_raster(const YAML::Node &node); 

    // Set the period for a single location given the index
    void set_seasonal_period(const YAML::Node &node, int index);
};

namespace Spatial {
class SpatialModel;
}

class spatial_model : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(spatial_model)

 DISALLOW_MOVE(spatial_model)

 public:
  Spatial::SpatialModel *value_{nullptr};
 public:
  //constructor
  explicit spatial_model(const std::string &name, Spatial::SpatialModel *default_value, Config *config = nullptr) :
      IConfigItem(config, name),
      value_{default_value} {}

  // destructor
  virtual ~spatial_model();

  virtual Spatial::SpatialModel *operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class immune_system_information : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(immune_system_information)

 DISALLOW_MOVE(immune_system_information)

 public:
  ImmuneSystemInformation value_;
 public:
  //constructor
  explicit immune_system_information(const std::string &name, ImmuneSystemInformation default_value,
                                     Config *config = nullptr) :
      IConfigItem(config, name),
      value_{std::move(default_value)} {}

  // destructor
  virtual ~immune_system_information() = default;

  virtual ImmuneSystemInformation &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class genotype_db : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(genotype_db)

 DISALLOW_MOVE(genotype_db)

 public:
  GenotypeDatabase *value_;
 public:
  //constructor
  explicit genotype_db(const std::string &name, GenotypeDatabase *default_value, Config *config = nullptr)
      : IConfigItem(config, name),
        value_{default_value} {}

  // destructor
  virtual ~genotype_db();

  virtual GenotypeDatabase *operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class number_of_parasite_types : public ConfigItem<unsigned long> {
 public:
  number_of_parasite_types(const std::string &name, const unsigned long &default_value,
                           Config *config) : ConfigItem<unsigned long>(name, default_value, config) {}

  void set_value(const YAML::Node &node) override;
};

class drug_db : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(drug_db)

 DISALLOW_MOVE(drug_db)

 public:
  DrugDatabase *value_;
 public:
  //constructor
  explicit drug_db(const std::string &name, DrugDatabase *default_value, Config *config = nullptr) : IConfigItem(
      config, name),
                                                                                                     value_{
                                                                                                         default_value} {}

  // destructor
  ~drug_db() override;

  virtual DrugDatabase *operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class EC50_power_n_table : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(EC50_power_n_table)

 DISALLOW_MOVE(EC50_power_n_table)

 public:
  DoubleVector2 value_;
 public:
  //constructor
  explicit EC50_power_n_table(const std::string &name, DoubleVector2 default_value, Config *config = nullptr)
      : IConfigItem(config, name),
        value_{
            std::move(default_value)
        } {}

  // destructor
  virtual ~EC50_power_n_table() = default;

  virtual DoubleVector2 &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class circulation_info : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(circulation_info)

 DISALLOW_MOVE(circulation_info)

 public:
  RelativeMovingInformation value_;
 public:
  //constructor
  explicit circulation_info(const std::string &name, RelativeMovingInformation default_value,
                            Config *config = nullptr) :
      IConfigItem(config, name),
      value_{std::move(default_value)} {}

  // destructor
  virtual ~circulation_info() = default;

  virtual RelativeMovingInformation &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class relative_bitting_info : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(relative_bitting_info)

 DISALLOW_MOVE(relative_bitting_info)

 public:
  RelativeBittingInformation value_;
 public:
  //constructor
  explicit relative_bitting_info(const std::string &name, RelativeBittingInformation default_value,
                                 Config *config = nullptr) :
      IConfigItem(config, name),
      value_{std::move(default_value)} {}

  // destructor
  virtual ~relative_bitting_info() = default;

  virtual RelativeBittingInformation &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class therapy_db : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(therapy_db)

 DISALLOW_MOVE(therapy_db)

 public:
  TherapyPtrVector value_;
 public:
  //constructor
  explicit therapy_db(const std::string &name, TherapyPtrVector default_value, Config *config = nullptr)
      : IConfigItem(config, name),
        value_{
            std::move(default_value)
        } {}

  // destructor
  virtual ~therapy_db();

  virtual TherapyPtrVector &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class strategy_db : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(strategy_db)

 DISALLOW_MOVE(strategy_db)

 public:
  StrategyPtrVector value_;
 public:
  //constructor
  explicit strategy_db(const std::string &name, StrategyPtrVector default_value, Config *config = nullptr)
      : IConfigItem(config, name),
        value_{
            std::move(default_value)
        } {}

  // destructor
  virtual ~strategy_db();

  virtual StrategyPtrVector &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class initial_parasite_info : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(initial_parasite_info)

 DISALLOW_MOVE(initial_parasite_info)

 public:
  std::vector<InitialParasiteInfo> value_;
 public:
  //constructor
  explicit initial_parasite_info(const std::string &name, std::vector<InitialParasiteInfo> default_value,
                                 Config *config = nullptr) :
      IConfigItem(config, name),
      value_{std::move(default_value)} {}

  // destructor
  virtual ~initial_parasite_info() = default;

  virtual std::vector<InitialParasiteInfo> &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class bitting_level_generator : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(bitting_level_generator)

 DISALLOW_MOVE(bitting_level_generator)

 public:
  MultinomialDistributionGenerator value_;
 public:
  //constructor
  explicit bitting_level_generator(const std::string &name, MultinomialDistributionGenerator default_value,
                                   Config *config = nullptr) : IConfigItem(config, name),
                                                               value_{std::move(default_value)} {}

  // destructor
  virtual ~bitting_level_generator() = default;

  virtual MultinomialDistributionGenerator &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class moving_level_generator : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(moving_level_generator)

 DISALLOW_MOVE(moving_level_generator)

 public:
  MultinomialDistributionGenerator value_;
 public:
  //constructor
  explicit moving_level_generator(const std::string &name, MultinomialDistributionGenerator default_value,
                                  Config *config = nullptr) : IConfigItem(config, name),
                                                              value_{std::move(default_value)} {}

  // destructor
  virtual ~moving_level_generator() = default;

  virtual MultinomialDistributionGenerator &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

class start_of_comparison_period : public ConfigItem<int> {
 public:
  start_of_comparison_period(const std::string &name, const int &default_value, Config *config) : ConfigItem<int>(
      name, default_value, config) {}

  void set_value(const YAML::Node &node) override;
};

struct beta_distribution_params {
  double alpha;
  double beta;
};

class prob_individual_present_at_mda_distribution : public IConfigItem {
 DISALLOW_COPY_AND_ASSIGN(prob_individual_present_at_mda_distribution)

 DISALLOW_MOVE(prob_individual_present_at_mda_distribution)

 public:
  std::vector<beta_distribution_params> value_;
 public:
  //constructor
  explicit prob_individual_present_at_mda_distribution(const std::string &name,
                                                       std::vector<beta_distribution_params> default_value,
                                                       Config *config = nullptr) : IConfigItem(config, name),
                                                                                   value_{std::move(default_value)} {}

  // destructor
  virtual ~prob_individual_present_at_mda_distribution() = default;

  virtual std::vector<beta_distribution_params> &operator()() {
    return value_;
  }

  void set_value(const YAML::Node &node) override;
};

#endif
