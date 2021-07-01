
# MaSim Configuration File

### Introduction

The malaria simulation (MaSim) uses [YAML](https://yaml.org/) to load configuration settings for the simulation. While the configuration was previously forward compatible (i.e., older files would work with newer versions) With the transition to version 4.0 new nodes were added or deprecated that have resulted in a divergence between 3.x and 4.0 onwards. While the absence of nodes implies version 4.0 or greater, care should be taken to ensure it is clear which version a given configuration is intended for.

# Nodes

## Model Operation Nodes 
The following nodes govern how the model executes at a low level.

**connection_string** (string) : (*Version 4.0*) The connection string for the PostgreSQL database that stores the simulation data.

**record_genome_db** (boolean) : (*Version 4.0*) Indicates that genome data should be recorded to the database when using the `DbReporter` reporter class. Note that recording genomic data to the database will cause the database to quickly inflate in size. It is recommended that this setting only be used when genomic data needs to be retrieved. 

## Model Configuration Notes
The following nodes contain the settings for the simulation.

**transmission_parameter** (float) : Governs how likely malaria is to be transmitted to a naive individual in the sporozoite challenge.

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

### parasite_density_level
The `parasite_density_level` setting contains several sub-values that govern indivdiual behavior or state due to the total number of parasites that are present in the individual's blood stream. When setting the parasite density for the detectable levels note that 10 per μl is the middle of the bounds for detection under [Giemsa-stained thick blood film](https://apps.who.int/iris/bitstream/handle/10665/274382/MM-SOP-07a-eng.pdf) under laboratory conditions (4 - 20 parasites/μl) while 50 per μl is the lower bounds for detection under field conditions (50 - 100 parasites/μl) ([Wongsrichanalai et al. 2007](#Wongsrichanalai2007)). Generally, a higher detection limit for the <em>Pf</em>PR will require a higher tranmission for a given <em>Pf</em>PR than a lower detection level. 

```YAML
parasite_density_level:
  log_parasite_density_cured:          -4.699    # corresponds to 100 total parasites (0.00002 per μl)
  log_parasite_density_from_liver:     -2.000    # corresponds to 50,000 total parasites (0.01 per μl)
  log_parasite_density_asymptomatic:    3        # corresponds to 1,000 parasites per microliter of blood
  log_parasite_density_clinical:        4.301    # corresponds to 20,000 parasites per microliter of blood (total 10^11)
  log_parasite_density_clinical_from:   3.301    # corresponds to 2,000 parasites per microliter of blood (total 10^10)
  log_parasite_density_clinical_to:     5.301    # corresponds to 200,000 parasites per microliter of blood (total 10^12)
  log_parasite_density_detectable:      1.000    # corresponds to 10 parasites per microliter of blood
  log_parasite_density_detectable_pfpr: 1.699    # corresponds to 50 parasites per microliter of blood
  log_parasite_density_pyrogenic:       3.398    # corresponds to 2,500 parasites per microliter of blood
```

**log_parasite_density_cured** (double) : When an indivdiual is considered to be **cured** of a specific parasite colony. \
**log_parasite_density_from_liver** (double) : Governs the lower bound for the number of parasites following the inital infection. \
**log_parasite_density_asymptomatic** (double) : Thereshold at which an indivdiual is asymtomatic of malaria. \
**log_parasite_density_clinical** (double) : Governs the upper bound for the number of parasites following the initial infection. \
**log_parasite_density_clinical_from** (double) : Governs the lower bound of parasites that an indivdiual may be inflicted with when progressing to clinical via the `ProgressToClinicalEvent` event. \
**log_parasite_density_clinical_to** (double) : Governs the upper bound of parasites that an indivdiual may be inflicted with when progressing to clinical via the `ProgressToClinicalEvent` event. \
**log_parasite_density_detectable** (double) : Sets the threshold for the number of parasties that an indivdiual may have present in their blood when tested to check if the perscribed treatment failed. \
**log_parasite_density_detectable_pfpr** (double) : Sets the threshold for the number of parasites that an indivdual may have present in their blood when tested to see if a _detectable_ level is presented. This value is used to inform calculations for the <em>Pf</em>PR in the simuation. \
**log_parasite_density_pyrogenic** (double) : (**UNUSED**) Sets the threshold for when fever may present as a symptom.


### immune_system_information
The `immune_system_information` node contains parameters that are used to control the response of the individual immune system and is one of the mechanisms by which the simulation can be calibrated to match a given country.  

```YAML
immune_system_information:
  # Immune function parameters
  b1: 0.00125
  b2: 0.0025

  # Duration of infection parameters
  duration_for_naive: 300
  duration_for_fully_immune: 60

  # Population initialization parameters
  mean_initial_condition: 0.1
  sd_initial_condition: 0.1

  # Probability bounds for clinical symptoms
  min_clinical_probability: 0.05
  max_clinical_probability: 0.99

  # Immunity acquisition parameters
  immune_inflation_rate: 0.01
  age_mature_immunity: 10
  factor_effect_age_mature_immunity: 0.3

  # Immunity function parameters
  immune_effect_on_progression_to_clinical: 12
  midpoint: 0.4  
```
  
**b1** (double) : Rate of immune function increase when parasitaemic \
**b2** (double) : Rate of immune function decrease when not parasitaemic \
**duration_for_naive** (double) : Duration, in days, of infection when naive. \
**duration_for_fully_immune** (double) : Duration, in days, of infection when fully immune. \
**mean_initial_condition** (double) : Mean initial immune function of population at initialization. \
**sd_initial_condition** (double) : Standard deviation of initial immune function of population at initiation. \
**min_clinical_probability** (double) : Minimum probability of clinical symptoms as a result of a new infection. \
**max_clinical_probability** (double) : Maximum probability of clinical symptoms as a result of a new infection. \
**immune_inflation_rate** (double) : Yearly age-dependent faster acquisition of immunity between ages 1 to 10. \
**age_mature_immunity** (double) : Age at which the immune function is mature, i.e., age at which the immune acquisition model switches from child to adult. \
**factor_effect_age_mature_immunity** (double) :  Adjustment to the curve of immune acquiziatoin under the age indicated by `age_mature_immunity`, parameter kappa in supplement to Nguyen et al. (2015). \
**immune_effect_on_progression_to_clinical** (double) : Slope of the sigmoidal probability versus immunity function, parameter z in supplement to Nguyen et al. ([2015](#Nguyen2015)). \
**midpoint** (double) : (*Version 4.0*) Adjusts the midpoint of the slope of the sigmoidal probability versus immunity function, parameter z in supplement to Nguyen et al. ([2015](#Nguyen2015)).


### drug_db
This setting is used to configure the various drugs used in the configuration and is structured as an array of drugs, which contain the specific setting for that particular compound. **Note** that while the simuation assumes that the compounds will be ordered from 0 to *n*, it is the reponsilbity of the user to ensure that they are assigned and ordered correctly.

```YAML
drug_db:
  0:
    name: "ART"       # Artemisinin
    half_life: 0.0
    maximum_parasite_killing_rate: 0.999
    n: 25
    age_specific_drug_concentration_sd: [0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4]
    age_specific_drug_absorption: [0.7,0.7,0.85,0.85,0.85,0.85,0.85,0.85,0.85,0.85,1.0,1.0,1.0,1.0,1.0]
    mutation_probability: 0.005
    affecting_loci: [2]
    selecting_alleles: [[1]]
    k: 4
    EC50:
      ..0..: 0.75
      ..1..: 1.2
```      

**name** (string) : The name of the compound. \
**half_life** (double) : The compound's half-life in the body, in days. \
**maximum_parasite_killing_rate** (double) : The percentage of parasites that the compound will kill in one day if an individual has the highest possible drug concentration. \
**n** (integer) : The slope of the linear porition of the concentration-effect curve. \
**age_specific_drug_concentration_sd** (double array) : The actual drug concentration, per individual, that will be drawn from a normal distribution with an mean of one and this standard deviation. \
**age_specific_drug_absorption** (double array) : (*Version 4.0*) The percentage of the drug that is absorbed into the bloodstream, based upon the age of the individual. When not supplied the default value is one for all age groups. \
**mutation_probability** (double) : The probablity that exposure to the drug will result in a muatation in the parasite to resist it. \
**affecting_loci** (integer array) : The index of the loci of alleles where drug resistnace may form (see [genotype_info](#genotype_info)). \
**selecting_alleles** (integer matrix) : The index of the alleles where drug resistance may form (see [genotype_info](#genotype_info)). \
**k** (double) : Controls the change in the mutation probability when drug levels are intermediate. For example, k=0.5 is a simple linear model where mutation probability decreases linearly with drug concentration; where as k=2 or k=4 are a piecewise-linear model where mutation probability increases from high concentrations to intermediate concentrations, and then decreases linearly from intermediate concentrations to zero. \
**EC50** (array of key-value pairs) : The drug concentration which produces 50% of the parasite killing achieved at maximum-concentration, format is is a string that descives the relevent genotypes (see [genotype_info](#genotype_info)), followed by the concentration where 1.0 is the expected starting concentration.


### genotype_info<a name="genotype_info"></a>
To Be Written.


# events
This setting is used to list the various events that will be loaded and run during the model. The `name` field dictates which event will be parsed and all of the data for the `info` field following will be provided to the loader function.

#### annual_beta_update_event
(*Version 4.0*) The annual beta update event increases or decreases the beta for each cell in the model using the formula `beta' = beta + (beta * rate)` and clamps the lower bounds for the beta a zero. 

```YAML
events:
  - name: annual_beta_update_event
    info:
      - day: 2020/10/26
        rate: -0.025
```

**day** (date string, YYYY/mm/dd) : the date when the update will first occur. \
**rate** (float) : the rate of change for the beta for the cells.

#### annual_coverage_update_event 
(*Version 4.0*) The annual coverage update event increases the coverage for each cell in the model by reducing the coverage gap by a fixed value provided by rate. If the model is run for a long enough period of time, the presumption should be that the coverage will reach 100%.

```YAML
events:
  - name: annual_coverage_update_event
    info:
      - day: 2020/09/02
        rate: 0.025
```

**day** (date string, YYYY/mm/dd) : the date when the update will first occur. \
**rate** (float) : the rate of reduction in the coverage for the cells.

#### importation_periodically_random_event
(*Version 4.0*) Introduce a new malaria infection into the model at a random location, which has been selected using a draw that is weighted by the population. After the initial event, it will be rescheduled every *n* days based upon the periodicity.

```YAML
events:
  - name: importation_periodically_random_event
    info:
      - day: 2020/10/26
        genotype_id: 1
        periodicity: 30
      - day: 2020/10/26
        genotype_id: 2
        periodicity: 365
```

**day** (date string, YYYY/mm/dd) : the date when the first event will occur. \
**genotype_id** (int) : the id of the genotype to be introduced. \
**log_parasite_density** (double) : the log density of the parasite to be imported. \
**periodicity** (int) : the number of days between events after the first one.

#### turn_off_mutation
Turn off all mutations in the model, recommended during the model burn-in.

```YAML
events:
  - name: turn_off_mutation
    info:
      - day: 2020/09/04
```

**day** (date string, YYYY/mm/dd) : the date when the event will occur.

#### turn_on_mutation
Turn on all mutations in the model, or the mutations for individual drugs.

```YAML
events:
  - name: turn_on_mutation
    info:
      - day: 2020/09/04
        drug_id: 0
        mutation_probability: 0.005
```

**day** (date string, YYYY/mm/dd) : the date when the event will occur. \
**drug_id** (integer) : the id of the drug, as defined in the `drug_db` or `-1` to apply the value to all drugs. \
**mutation_probability** (float) : the mutation probability to use.

#### update_ecozone_event
(*Version 4.0*) Update all of the cells matching the original ecozone to the new ecozone.

```YAML
events:
  - name: update_ecozone_event
    info:
      - day: 2021/01/23
        from: 0
        to: 1
```

**day** (date string, YYYY/mm/dd) : the date when the event will occur. \
**from** (integer) : the id of the ecozone, defined in `seasonal_info`, that is the original ecozone. \
**to** (integer) : the id of the new ecozone, defined in `seasonal_info`, that will be applied to matching cells.

## References
<a name="Nguyen2015"></a>Nguyen, TD, Olliaro, P, Dondorp, AM, Baird, JK, Lam, HM, Farrar, J, Thwaites, GE, White, NJ, & Boni, MF. (2015). Optimum population-level use of artemisinin combination therapies: A modelling study. *The Lancet Global Health*, 3(12), e758–e766. https://doi.org/10.1016/S2214-109X(15)00162-X

<a name="Wongsrichanalai2007"></a>Wongsrichanalai C, Barcus MJ, Muth S, et al. A Review of Malaria Diagnostic Tools: Microscopy and Rapid Diagnostic Test (RDT) In: Breman JG, Alilio MS, White NJ, editors. Defining and Defeating the Intolerable Burden of Malaria III: Progress and Perspectives: Supplement to Volume 77(6) of American Journal of Tropical Medicine and Hygiene. Northbrook (IL): American Society of Tropical Medicine and Hygiene; 2007 Dec. Available from: https://www.ncbi.nlm.nih.gov/books/NBK1695/
