# PSU-CIDD-Malaria-Simulation

[Penn State](https://www.psu.edu/) - [Center for Infectious Disease Dynamics (CIDD)](https://www.huck.psu.edu/institutes-and-centers/center-for-infectious-disease-dynamics) - [Boni Lab](http://mol.ax/)

---

## Overview

This repository contains the working codebase for the Malaria Simulation (MaSim) under development by the Boni Lab at Penn State. While efforts are made to keep releases organized, stable code specific to publications are maintained in repositories under the [Boni Lab on GitHub](https://github.com/bonilab). Instructions for installing the tool chains required for development can be found under [documentation/developer.md](documentation/developer.md) and basic instructions for running the simulation can be found in [documentation/demo/](documentation/demo/).

The simulation has been tested to run on Windows 10, Windows Subsystem for Linux (Ubuntu), and Red Hat 7.9. The majority of development is performed on under Linux so building and running under Windows may be impacted.  While basic simulations are possible on desktop computing environments, regional and national scale simulations require advanced computing environments with access to 64 GB of RAM or more. Sample configuration files can be found under [documentation/input/](documentation/input), and examination of `simple.yml` or `spatial.yml` is recommended after working with the demonstration configuration in [documentation/demo/](documentation/demo/).

## Command Line Switches

The following commands are available from the simulation:
<pre>
-c / --config     Configuration file, variant flag 
-h / --help       Display this help menu
-i / --input      Configuration file, preferred flag
-j                The job number for this replicate
-l / --load       Load genotypes to the database and exit
-o                The path for output files, default is the current directory
-r                The reporter type to use, multiple supported when comma delimited
-s                The study number to associate with the configuration

--dump            Dump the movement matrix as calculated
--im              Record individual movement detail
--lg              List the possible genotypes and their internal id values
--lr              List the possible data reporters
--mc              Record the movement between cells, cannot run with --md
--md              Record the movement between districts, cannot run with --mc

--v=[int]         Sets the verbosity of the logging, default zero
</pre>

Use of either the `-c` or `-i` switch with an appropriate YAML file is required. When the `-r` switch is not supplied the simulation defaults to the `DbReporter`; however, with the `-r` switch the reporters listed using the `--lr` switch can be used instead.

## Releases

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
