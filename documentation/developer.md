# Introduction

This file outlines the basic steps that are needed to build the simulation across various development environments. The guide is written from the standpoint of a "clean" workstation with no other dependencies in place.

# Tool Chain Dependencies

## Windows 10 / Windows Subsystem for Linux (WSL)
1. Install a Linux sub-system of your choice (Ubuntu recommended, or Red Hat)

2. Update the Linux sub-system
```bash
sudo apt update
sudo apt upgrade
```

3. Install the build dependencies
```bash
sudo apt install build-essentials
sudo apt install cmake
sudo apt install libgsl-dev
sudo apt install libyaml-cpp-dev
sudo apt install libfmt-dev
sudo apt upgrade
```

4. Install the PostgreSQL and libpqxx dependencies

Note that this should be done from a directory that you are comfortable with git repositories being stored in.
```bash
sudo apt install postgresql-10
sudo apt upgrade
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
./configure --disable-documentation
make
sudo make install
```

## WSL on the PSU VPN
One minor problem that may occure while on the PSU VPN is that psu.edu domains are not resolved correctly. This is usually due to the `/etc/resolv.conf` file not being updated correctly by WSL. To manually update the file, launch PowerShell and run,

```PowerShell
Get-DnsClientServerAddress -AddressFamily IPv4 | Select-Object -ExpandPropert ServerAddresses
```

copy the addresses to `/etc/resolv.conf` so you have something similar to 

```
nameserver 192.168.1.1
nameserver 192.168.1.2
nameserver 192.168.1.3
search psu.edu
```

This may need to be done everytime you connect to the VPN, so scripting the update may be in order.

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

Note that while care was taken in places to ensure the code is performant, the amount of RAM needed during execution can be quite high (ex., 32GB or more). When the model is run in Linux enviroments where the necessary memeory is not avalible, you may find that the program is killed without notice due to being [out of memory](https://linux-mm.org/OOM_Killer).

# Building on ICDS-ACI
To build the simulation to run on the ICDS-ACI the first time it is necessary to perform a number of configuration steps. After logging on to the interactive environment (`aci-b.aci.ics.psu.edu`) cloning this repository run `config.sh` which will prepare the build environment. As part of the process a build script will be created at `build/build.sh` that will ensure the environment is set correctly when run. 

# Development Tools

## Isolating Segmentation Faults in Linux
When developing new functionality in the model it is sometimes necessary to isolate segmentation faults. One of the easier ways to do this is through the use of `gdb` in a Linux environment. First, compile the program with the `debug` flag set, this will ensure that there are debug symbols in the binary. Then use `gdb` to open the gdb console:

```bash
file bin/MaSim              # Load the specified executable
run -i ../input/sample.yml  # Run the program with the following command line parameters

...                         # Program output omitted

bt                          # Generates the stack trace
```

## Profiling
While there are many approaches to code profiling, as a quick way to get up and running, [valgrind](http://www.valgrind.org/) is recommended along with [gprof2dot](https://github.com/jrfonseca/gprof2dot). Following installation, the simulation can be profiled using:

```bash
valgrind --tool=callgrind ./bin/MaSim - ../input/sample.yml
gprof2dot -f callgrind callgrind.out.* | dot -Tpng -o output.png
```

This will generate a PNG file containing a node graph of where most of the simulation's time is spent during execution along with the percentage of time spent in a given function. 

Note that `valgrind` adds **considerable** overhead to the execution of the simulation so it is highly recommended that profiling be limited to small simulations. 

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
