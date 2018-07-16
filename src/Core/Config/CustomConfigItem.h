#ifndef CUSTOMCONFIGITEM_H
#define CUSTOMCONFIGITEM_H

#include <string>
#include <utility>
#include "ConfigItem.h"
#include "Therapies/DrugDatabase.h"
#include "Parasites/GenotypeDatabase.h"


namespace YAML {
  class Node;
}

class Config;

class total_time : public ConfigItem<int> {
public:
  total_time(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};

class number_of_age_classes : public ConfigItem<int> {
public:
  number_of_age_classes(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};

class number_of_locations : public ConfigItem<int> {
public:
  number_of_locations(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}
  void set_value(const YAML::Node& node) override;
};

class spatial_distance_matrix : public ConfigItem<std::vector<std::vector<double>>> {
public:
  spatial_distance_matrix(std::string name, const std::vector<std::vector<double>>& default_value, Config* config) : ConfigItem<std::vector<
    std::vector<double>>>(
    name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};


class seasonal_info : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(seasonal_info)
DISALLOW_MOVE(seasonal_info)
protected:
  SeasonalInfo value_{};
public:
  //constructor
  explicit seasonal_info(std::string name, SeasonalInfo default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                                   value_{std::move(default_value)} { }

  // destructor
  virtual ~seasonal_info() = default;


  virtual SeasonalInfo& operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

namespace Spatial {
  class SpatialModel;
}

class spatial_model : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(spatial_model)
DISALLOW_MOVE(spatial_model)
protected:
  Spatial::SpatialModel* value_{nullptr};
public:
  //constructor
  explicit spatial_model(std::string name, Spatial::SpatialModel* default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                                             value_{default_value} { }

  // destructor
  virtual ~spatial_model();

  virtual Spatial::SpatialModel* operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};


class immune_system_information : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(immune_system_information)
DISALLOW_MOVE(immune_system_information)
protected:
  ImmuneSystemInformation value_;
public:
  //constructor
  explicit immune_system_information(std::string name, ImmuneSystemInformation default_value, Config* config = nullptr) :
    IConfigItem(config, name),
    value_{std::move(default_value)} { }

  // destructor
  virtual ~immune_system_information() = default;

  virtual ImmuneSystemInformation& operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

class genotype_db : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(genotype_db)
DISALLOW_MOVE(genotype_db)
protected:
  GenotypeDatabase* value_;
public:
  //constructor
  explicit genotype_db(std::string name, GenotypeDatabase* default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                                         value_{default_value} { }

  // destructor
  virtual ~genotype_db();

  virtual GenotypeDatabase* operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};


class number_of_parasite_types : public ConfigItem<int> {
public:
  number_of_parasite_types(std::string name, const int& default_value, Config* config) : ConfigItem<int>(name, default_value, config) {}

  void set_value(const YAML::Node& node) override;
};

class drug_db : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(drug_db)
DISALLOW_MOVE(drug_db)
protected:
  DrugDatabase* value_;
public:
  //constructor
  explicit drug_db(std::string name, DrugDatabase* default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                              value_{default_value} { }

  // destructor
  virtual ~drug_db();

  virtual DrugDatabase* operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

class EC50_power_n_table : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(EC50_power_n_table)
DISALLOW_MOVE(EC50_power_n_table)
protected:
  DoubleVector2 value_;
public:
  //constructor
  explicit EC50_power_n_table(std::string name, DoubleVector2 default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                              value_{std::move(default_value)} { }

  // destructor
  virtual ~EC50_power_n_table() = default;

  virtual DoubleVector2& operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

class circulation_info : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(circulation_info)
DISALLOW_MOVE(circulation_info)
protected:
	RelativeMovingInformation value_;
public:
  //constructor
  explicit circulation_info(std::string name, RelativeMovingInformation default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                              value_{std::move(default_value)} { }

  // destructor
  virtual ~circulation_info() = default;

  virtual RelativeMovingInformation& operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

class relative_bitting_info : public IConfigItem {
DISALLOW_COPY_AND_ASSIGN(relative_bitting_info)
DISALLOW_MOVE(relative_bitting_info)
protected:
	RelativeBittingInformation value_;
public:
  //constructor
  explicit relative_bitting_info(std::string name, RelativeBittingInformation default_value, Config* config = nullptr) : IConfigItem(config, name),
                                                                                              value_{std::move(default_value)} { }

  // destructor
  virtual ~relative_bitting_info() = default;

  virtual RelativeBittingInformation& operator()() {
    return value_;
  }

  void set_value(const YAML::Node& node) override;
};

class therapy_db : public IConfigItem {
	DISALLOW_COPY_AND_ASSIGN(therapy_db)
		DISALLOW_MOVE(therapy_db)
protected:
	TherapyPtrVector value_;
public:
	//constructor
	explicit therapy_db(std::string name, TherapyPtrVector default_value, Config* config = nullptr) : IConfigItem(config, name),
		value_{std::move(default_value)} { }

	// destructor
	virtual ~therapy_db();

	virtual TherapyPtrVector& operator()() {
		return value_;
	}

	void set_value(const YAML::Node& node) override;
};

#endif // CUSTOMCONFIGITEM_H
