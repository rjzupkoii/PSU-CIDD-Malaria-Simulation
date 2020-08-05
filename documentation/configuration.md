
# MaSim Configuration File

## Introduction

The malaria simulation (MaSim) uses [YAML](https://yaml.org/) to load configuration settings for the simulation. While the configuration was previously forward compatible (i.e., older files would work with newer versions) With the transition to version 4.0 new nodes were added or deprecated that have resulted in a divergence between 3.x and 4.0 onwards. While the absence of nodes implies version 4.0 or greater, care should be taken to ensure it is clear which version a given configuration is intended for.

## Nodes

### Model Operation Nodes 
The following nodes govern how the model executes at a low level.

**connection_string** (string) : (*Version 4.0*) The connection string for the PostgreSQL database that stores the simulation data.


### Model Configuration Notes
The following nodes contain the settings for the simulation.

**transmission_parameter** (float) : Governs how likely malaira is to be transmitted to a naive individual in the sporozoite challenge.

**number_of_age_classes** (integer) : The size of the `age_structure` array.\
**age_structure** : An array of integer values that corresponds to the oldest age that defines a break in the age structure.


### raster_db
*Version 4.0* This node contains data related to the the spatial organization of the model to include population distributions and raster files.

*Usage*\
The use of the `raster_db` node will override the use of the `location_db` and errors or inconsistences will occurs if both are used at the same time. All of the raster files must have the same header information as defined in the [Esri ASCII raster format](https://desktop.arcgis.com/en/arcmap/10.3/manage-data/raster-and-images/esri-ascii-raster-format.htm) and the same number of defined pixels. A `std::runtime_error` may be generated if the number of data pixels in a given raster exceeds, or is less than a previously loaded raster.

Note that while any arbitrary value may be used for the `NODATA_VALUE` it is recommended that the standard value of `-9999` be used.

```YAML
raster_db:
  beta_raster: "beta.asc"
  district_raster: "district.asc"
  location_raster: "location.asc"
  population_raster: "population.asc"
  travel_raster: "travel.asc"
  ecoclimatic_raster: "ecoclimatic.asc"
  pr_treatment_under5: "pr_treatment_under5.asc"
  pr_treatment_over5: "pr_treatment_over5.asc"

  cell_size: 5
  age_distribution_by_location: [[0.0]]
  p_treatment_for_less_than_5_by_location: [0.0]
  p_treatment_for_more_than_5_by_location: [0.0]

  beta_by_location: [0.0]
```
**beta_raster** (string) : The beta parameter (float) used for each cell in the model, overrides the value set in `beta_by_location`.\
**district_raster** (string) : Contains the canonical identification values (integer) that should be for each cell in the model.\
**location_raster** (string) : May contain arbitrary numeric values and is used by the simulation to convert the X-Y coordinates to their linear values. Only needed if it is the *only* raster file provided.\
**population_raster** (string) : Contains the number of individuals (integer) in each cell within the simulation.\
**travel_raster** (string) : Contains the friction surface (float) that is used for determining the likelihood of travel to a given cell.\
**ecoclimatic_raster** (string) : Contains the ecoclimatic zone value for each cell. The ecoclimatic zone value should be a zero-indexed integer that corresponds to the `seasonal_info` value to use.\
**pr_treatment_under5** (string) : Contains the access to treatment (float) for individuals under five years (60 months) for each cell in the simulation, overrides `p_treatment_for_less_than_5_by_location`.\
**pr_treatment_over5** (string) : Contains the access to treatment (float) for individuals over five years (60 months) for each cell in the simulation, overrides `p_treatment_for_more_than_5_by_location`.

**cell_size** (float) : The size of each cell along one axis, in kilometers.\
**age_distribution_by_location** : An array of arrays which contains floating point values corresponding to the age distribution (as defined by `age_structure`).\
**p_treatment_for_less_than_5_by_location**, **p_treatment_for_more_than_5_by_location** : (*Deprecated in Version 4.0*) Contains the access to treatment for individuals under or over (respectively) five years (60 months). If the array length is one, then the value supplied (float) is used for all locations in the model. Otherwise, each element in the array is presumed to correspond to a different location. 

**beta_by_location** : (*Array deprecated in Version 4.0, single value recommended or raster*) Contains the beta value (float) to be used in the simulation. If the array length is one, then the value supplied (float) is used for all locations in the model. Otherwise, each element in the array is presumed to correspond to a different location. 

### location_db
To Be Written.


### seasonal_info 
This setting governs the malaria season in the model.

```YAML
seasonal_info:
  enable: true
  raster: false
  a: [0.0]
  phi: [0.0]
  min_value: [0.0]
  period: 365
```

**enable** (true | false) : enables or disables seasonality in the simulation.\
**raster** (true | false) : (*Version 4.0*) indicates that a raster file should be used to set the correct rate for each pixel.\
**a**, **phi**, **min_value** : arrays of one to *n* `double` values that inform the seasonality period.\
&nbsp;*Version 3.x*: when one value is provided, it is used for all locations, otherwise a value must be provided for each location\
&nbsp;*Version 4.0*: if `raster` is true then each index in the array is used for the pixel coded with that value, otherwise the first value is used for all pixels.\
**period** : the number of days defined by the period.
