
# MaSim Configuration File

## Introduction

The malaria simulation (MaSim) uses [YAML](https://yaml.org/) to load configuration settings for the simulation. While the configuration was previously forward compatible (i.e., older files would work with newer versions) With the transition to version 4.0 new nodes were added or deprecated that have resulted in a divergence between 3.x and 4.0 onwards. While the absence of nodes implies version 4.0 or greater, care should be taken to ensure it is clear which version a given configuration is intended for.

## Nodes

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
  *Version 3.x*: when one value is provided, it is used for all locations, otherwise a value must be provided for each location\
  *Version 4.0*: if `raster` is true then each index in the array is used for the pixel coded with that value, otherwise the first value is used for all pixels.\
**period** : the number of days defined by the period.
