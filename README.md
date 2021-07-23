# PSU-CIDD-Malaria-Simulation

[Penn State](https://www.psu.edu/) - [Center for Infectious Disease Dynamics (CIDD)](https://www.huck.psu.edu/institutes-and-centers/center-for-infectious-disease-dynamics) - [Boni Lab](http://mol.ax/)

---

## Introduction

This repository contains the working codebase for the Malaria Simuation (MaSim) under development by the Boni Lab at Penn State. While efforts are made to keep releases organized, stable code specific to publications are maintained in repostiories under the [Boni Lab on GitHub](https://github.com/bonilab). 


## Documentation

The simuation has been tested to run on Windows 10, Windows Subsystem for Linux (Ubuntu), and Red Hat 7.9. Instructions for installing the toolchains required for development can be found under [documentation/developer.md](documentation/developer.md) and basic instructions for running the simuation can be found in [documentation/demo/](documentation/demo/). While basic simuations are possible on desktop comuting enviornemnts, regional and national scale simuations require advanced computing enviorments with access to 64 GB of RAM or more.

---

The following commands are available from the simulation:
<pre>
-c / --config     Configuration file, variant flag 
-h / --help       Display this help menu
-i / --input      Configuration file, preferred flag
-j                The job number for this replicate
-l / --load       Load genotypes to the database and exit
-o                The path for output files, default is the current directory
-r                The reporter type to use
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


---

## About

### 4.x.main

The 4.x.main branch of MaSim marks a significant upgrade from previous versions, although backwards comparability has been maintained where possible. The major changes with this version of the model are:

1. Increased spatial support - models can now use a [ESRI ASCII Raster](http://resources.esri.com/help/9.3/arcgisengine/java/GP_ToolRef/spatial_analyst_tools/esri_ascii_raster_format.htm) for geographic data such as population distribution or a country's political organization.
2. Reporting of fine grained information to a PostgreSQL database.
3. Reporting of agent movement during model execution.
