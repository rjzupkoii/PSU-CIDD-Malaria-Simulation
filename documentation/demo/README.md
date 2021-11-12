# Demonstration Configuration

The configuration in this directory (`demo.yml`) is configured for a single cell and derives the majority of the settings from the Burkina Faso project, allowing it to emulate a realistic starting point for a model. In addition to the configuration file, there is also a basic raster file (`population.asc`) which informs the model on how to create a single cell with 1,000 individuals in it. The raster files used by the simulation are compliant with the [Esri ASCII raster format](https://desktop.arcgis.com/en/arcmap/10.3/manage-data/raster-and-images/esri-ascii-raster-format.htm) and can be edited using a text editor, or appropriate geographic information system (GIS) software.

In order to use this demonstration code you first need to build the simulation using the [directions outlined in developer.md](../developer.md#building), after which is recommended that you copy the binary (`MaSim`) and contents of this directory (e.g., `demo.yml`, `population.asc`) into the same location. The simulation can then be executed using the following command:

`./MaSim -i demo.yml -r CellularReporter`

Using the `CellularReporter` will generate a comma separated values (CSV) file in the directory containing model output for select parameters such as population, *Pf*PR<sub>2-10</sub>, and infected individuals to name a few. Running the simulation a second time will cause this file to be overwritten unless the job number (`-j`) switch is used. The `CellularReporter` is intended for models with only one location (i.e., cell) and will generate an error if multiple cells are present in the configuration.

## Column Descriptions

The following tables are present in the output from the `CellularReporter`:

| Column | Description |
| --- | --- |
| DaysElapsed	| Model days elapsed, should correspond to the length of months. |
| Population | Total population in the simulation. |
| PfPR2to10	| The *Plasmodium Falciparum* parasite prevalence for individuals aged 2 to 10 years. |
| InfectedIndividuals	| The number of individuals in the simuation who have *any* level of parasitemia. |
| ClinicalIndividuals	| The number of individuals in the simuation who have a parasitemia level of clinical symptoms to manifest. |
| ClinicalU5 | The number of clinical individuals under the age of 5. |
| ClinicalO5 | The number of clinical individuals over the age of 5. |
| NewInfections	| The number of new infections in the past month. |
| NonTreatment | The number of clinical individuals who did not seek treatment. |
| TreatmentFailure | The number of clinical individuals who sought treatment and the treatment did not clear the infection in the configured amount of time. |
| ParasiteClones | The number of parasite clones present in the simulation, one individual may be infected by multiple clones. |
| Theta	| The mean measure of immunity to the parasite in the simulation, zero means no immunity. |
| 580yWeighted | The weighted number of occurrences of the 580Y genotype, such that the sum of all clones in an individual equals one. | 
| 508yUnweighted | The total number of occurrences of the 580Y genotype in the simulation. |
| Plasmepsin2xCopyWeighted | The weighted number of occurrences of the Plasmepsin double copy mutation, such that the sum of all clones in an individual equals one. |
| Plasmepsin2xCopyUnweighted | The total number of occurrences of the Plasmepsin double copy mutation in the simulation. |


## Example Console Output

<pre>
[INFO] [default] MaSim version 4.0, experimental
[INFO] [default] Model initializing...
[INFO] [default] Read input file: demo.yml
[WARNING] [default] p_infection_from_an_infectious_bite used default value of 0
[INFO] [default] location_db appears to have been set by raster_db
[WARNING] [default] as_iov used default value of 0.2
[INFO] [default] Random initializing with seed: 1625174844985602
[INFO] [default] Starting day is 2007-01-01
[INFO] [default] Location count: 1
[INFO] [default] Population size: 1000
[INFO] [default] Model starting...
[INFO] [default] Perform before run events
[INFO] [default] Simulation is running
[INFO] [default] 17:27:24 - Day: 0
[INFO] [default] 2007-01-01 : turn mutation off
[INFO] [default] 17:27:25 - Day: 30
[INFO] [default] 17:27:25 - Day: 60
[INFO] [default] 17:27:25 - Day: 90

...

[INFO] [default] 17:28:19 - Day: 5400
[INFO] [default] 17:28:19 - Day: 5430
[INFO] [default] 17:28:19 - Day: 5460
[INFO] [default] Perform after run events
[INFO] [default] Model finished!
[INFO] [default] Final population: 1412
[INFO] [default] Elapsed time (s): 54.991
</pre>
