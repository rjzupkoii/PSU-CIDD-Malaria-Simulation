# Releases

The simulation does not follow a fixed release schedule and new versions are typically finalized when a publication is 
submitted for peer review. This also drives the feature that are added to the simulation since the current research needs
dictate the features currently under development.

## Change Log

### Version 4.1.5 (Development)

The version 4.1.5 release will be focused on adding features needed to allow for sub-national level interventions.

### Version 4.1.4

The version 4.1.4 release is focused on adding new features needed for research:

1. Added `update_beta_raster_event`

### Version 4.1.3

The version 4.1.3 release is focused on adding new features needed for research:

1. Add the relevant classes and events needed for the RAPT protocol.
2. Additional input validation added for input data files.
3. Updated console logging to report physical and virtual memory usage at end of simulation.

### Version 4.1.2.1

The version 4.1.2.1 release is focused on bug fixes and adding new features needed for research:

1. Removed the configuration field `p_compliance`.
2. Updated simulation and configuration to support variable treatment compliance rates based upon the day of treatment.

### Version 4.1.1.1

The version 4.1.1.1 release is specifically intended for work with manuscript revisions.

1. Updated the Marshall movement model to precompute the kernel.

### Version 4.1.1

The version 4.1.1 release is focused on adding new features needed for research:

1. Added new seasonality method based upon rainfall data.
2. Added new event (`introduce_mutant_event`) which allows mutations to be introduced in the popuation explicitly.
3. Added the genotype carriers reporter to the simulation.
4. Added the seasonal immunity reporter to the simulation.
5. Updated `DbReporter` to use ASCII character one (`SOH` or `☺`) to encode `\n\r` instead of removing them. Stored configurations can now have formatting restored.
6. Updated `-r` switch to support multiple comma delimited reporter types.
7. Refactored movement model for better flexibility, better Burkina Faso model performance.

### Version 4.1.0

The version 4.1.0 release focused on quality of life updates as well as correcting bugs found in the 4.0.0 release:

1. Added the `DbReporterDistrict` allowing for reporting data to be aggregated to the district level when reporting, database schema also updated to support this.
2. Weighted frequency has been removed from the genotype table (`sim.monthlygenomedata.weightedfrequency`), this will break code written for the 4.0.0 schema, but will reduce the storage requirements studies with many replicates and genotypes.
3. Deprecated values have been removed from the configuration (`min_clinical_probablity`).
4. Fixes for various bugs that were found in version 4.0.0.

### Version 4.0.0

The 4.0.0 release of MaSim marks a significant upgrade from previous versions, although backwards comparability has been maintained where possible. The major changes with this version of the model are:

1. Increased spatial support - models can now use a [ESRI ASCII Raster](http://resources.esri.com/help/9.3/arcgisengine/java/GP_ToolRef/spatial_analyst_tools/esri_ascii_raster_format.htm) for geographic data such as population distribution or a country's political organization.
2. Reporting of fine-grained information to a PostgreSQL database.
3. Reporting of agent movement during model execution.