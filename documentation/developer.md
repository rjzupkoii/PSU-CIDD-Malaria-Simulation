# Introduction
This file outlines the basic steps that are needed to build the simulation across various development environments. The guide is written from the standpoint of a "clean" workstation with no other dependenices in place. 

# Tool Chain Dependencies

## Windows 10 / Windows Subsystem for Linux (WSL) 
1. Install a Linux sub-system of your choice (Ubuntu recomended, or Red Hat)

2. Update the Linux sub-system
```bash
sudo apt update
sudo apt upgrade
```

3. Install the build dependenices
```bash
sudo apt install build-essentials
sudo apt install cmake
sudo apt install libgsl-dev
sudo apt install libyaml-cpp-dev
sudo apt install libfmt-dev
```

# Building
While building with `make` the `-j` switch may be used to control the number of cores used.

```bash
mkdir build
cd build
cmake .. 
make -j 4
```

# Running
To perform a basic model check the following can be run.

```bash
cd /build/bin
./MaSim -i ../../misc/input.yml
```

# Data Files
Several tab separated values (TSV) data files are generated depending upon the reporter that is selected for use. The files listed below are based upon the `MonthlyReporter` which generates two files. File(s) are organized with group separators indicated by the sentinel value `-1111` and *n* is a zero-indexed location id.

<br />

**monthly_reporter_*n*.txt** - Summary data for the model generated at the end of each simulated month.

| Block | Column Number(s) | Description |
| ---|--- | --- |
| Summary | 1 |  Model time, number of days elapsed |
| | 2 | Model time, calendar date as system time |
| | 3 - 5 | Model time, calendar date (Year, Month, Day) |
| | 6 | Seasonal factor |
| | 7 | Treatment coverage, probability to be treated (0 - 1) |
| | 8 | Treatment coverage, probability to be treated (0 - 10) |
| | 9 | Population size |
| | 10 | Group separator |
| EIR and PfRP | 1 + (*n* * 5) | EIR by location per year | 
| | 2 + (*n* * 5) | Group separator |
| | 3 + (*n* * 5) | Blood slide prevalence, PfPR < 5 |
| | 4 + (*n* * 5) | Blood slide prevalence, PfPR 2 - 2 | 
| | 5 + (*n* * 5) | Blood slide prevalence, PfPr all |
| Infections by Location | 1 | Group separator |
| | 2 + *n* | Number of new infections by location |
| Treatments by Location | 1 | Group separator |
| | 2 + *n* | Number of treatments by location |
| Clinical Episodes by Location | 1 | Group separator |
| | 2 + *n* | Number of clinical episodes by location |
| Genotype Frequency | 1 | Group separator |
| | *...* | See genotype frequency discussion

<br/>

**summary_reporter_*n*.txt** Summary that is generated after the model has completed execution.

| Block | Column Number(s) | Description |
| --- | --- | --- |
| Summary I | 1 | Random seed value |
|  | 2 | Number of locations | 
|  | 3 | *Beta* value |
|  | 4 | Population size |
| EIR and PfRP | 1 + (*n* * 5) | EIR by location per year | 
| | 2 + (*n* * 5) | Group separator |
| | 3 + (*n* * 5) | Blood slide prevalence, PfPR < 5 |
| | 4 + (*n* * 5) | Blood slide prevalence, PfPR 2 - 2 | 
| | 5 + (*n* * 5) | Blood slide prevalence, PfPr all |
| Summary II | 1 | Treatment strategy id |
| | 2 | Percent treatment failures per year | 


## Genotype Frequency
Genotype frequency is a complex entry that follows a similar output structure, but is generated differently depending upon the approach selected.

**Genotype frequency, by weighted number of parasite-positive individuals**
```
FOR EACH location
  FOR EACH state per person
    FOR EACH age class per person
      IF the person has no parasites THEN CONTINUE
      Update the total count
      Count the number of genotypes per person
      Adjust the count for the region
    END
  END
  // Formats the genotype per location as TSV
  FOR EACH genotype
    OUTPUT << weighted value << '\t'
  END
END
OUTPUT << "-1111\t"
// Format the summary genotype results as TSV
FOR EACH genotype
  OUTPUT << weighted value << 't'
END
OUTPUT << "-1111\t"
OUTPUT << Total count
```
