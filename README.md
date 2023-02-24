# PSU-CIDD-Malaria-Simulation

[Penn State](https://www.psu.edu/) - [Center for Infectious Disease Dynamics (CIDD)](https://www.huck.psu.edu/institutes-and-centers/center-for-infectious-disease-dynamics) - [Boni Lab](http://mol.ax/)

---

## Overview

This repository contains the working codebase for the Spaital Malaria Simulation (MaSim) under development by the Boni Lab at Penn State. The codebase was orginally forked from [maciekboni/PSU-CIDD-Malaria-Simulation](https://github.com/maciekboni/PSU-CIDD-Malaria-Simulation) and was detached in perperation of future development.

An [introduction to the model](documentation/README.md) can be found in the [documentation directory](documentation/). While instructions for installing the tool chains required for development can be found in the [developer documentation](documentation/developer.md) and basic instructions for running the simulation can be found in [the demo](documentation/demo/). Stable code specific to publications are maintained in repositories under the [Boni Lab on GitHub](https://github.com/bonilab). 

The simulation has been tested to run on Windows 10, Windows Subsystem for Linux (Ubuntu), and Red Hat 7.9. The majority of development is performed on under Linux so building and running under Windows may be impacted.  While basic simulations are possible on desktop computing environments, regional and national scale simulations require advanced computing environments with access to 64 GB of RAM or more. Sample configuration files can be found under [documentation/input/](documentation/input), and examination of `simple.yml` or `spatial.yml` is recommended after working with the demonstration configuration in [documentation/demo/](documentation/demo/).

## Command Line Arguments

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

