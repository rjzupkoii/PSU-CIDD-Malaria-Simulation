# PSU-CIDD-Malaria-Simulation

[Penn State](https://www.psu.edu/) - [Center for Infectious Disease Dynamics (CIDD)](https://www.huck.psu.edu/institutes-and-centers/center-for-infectious-disease-dynamics) - [Boni Lab](http://mol.ax/)

---

## About

The HRSpatial branch of the Malaria Simulation (MaSim) marks a significant upgrade from previous versions, although backwards comparability has been maintained where possible. The three key changes with this version of the model are:

1. Increased spatial support - models can now use a [ESRI ASCII Raster](http://resources.esri.com/help/9.3/arcgisengine/java/GP_ToolRef/spatial_analyst_tools/esri_ascii_raster_format.htm) for geographic data such as population distribution or a country's political organization.

2. Reporting of fine grained information to a PostgreSQL database.

3. Reporting of agent movement during model execution.

---

## Documentation

The following commands are available from the simulation:
<pre>
-h, --help                            Display this help menu
-i[string], -c[string],               The config file (YAML format); ex. MaSim -i input.yml
--input=[string],--config=[string]            
-j[int]                               Cluster job number; ex. MaSim -j 1
-r[string]                            Reporter Type; ex. MaSim -r mmc
-o[string]                            Path for output files, default is current directory; ex: MaSim -p out
--dump                                Dump the movement matrix as calculated
--im                                  Record individual movement detail
--mc                                  Record the movement between cells, cannot run with --md
--md                                  Record the movement between districts, cannot run with --mc
-l, --load                            Load the genotypes to the database
--v=[int]                             Sets the verbosity of the logging, default zero
</pre>
