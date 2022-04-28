# Introduction

This file outlines the basic steps that are needed to build the simulation across various development environments. The guide is written from the standpoint of a "clean" workstation with no other dependencies in place.

# Tool Chain Dependencies

## Windows 10 / Windows Subsystem for Linux (WSL 1)
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

4. Add PostgreSQL to the Apt Repository listing

Note that while this step is necessary when working from a new installation of Ubuntu, it may not be necessary, and you should check to see if the package can be found by first performing `apt search postgresql-10`

```bash
sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt-get update
```

5. Install the PostgreSQL and libpqxx dependencies

Note that this should be done from a directory that you are comfortable with git repositories being stored in.
```bash
sudo apt install postgresql-10
sudo apt upgrade
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
git checkout 7.0.0
./configure --disable-documentation
make
sudo make install
```

This may need to be done every time you connect to the VPN, so scripting the update may be in order.

## Upgrading to WSL 2

WSL 2 offers some [performance improvements](https://docs.microsoft.com/en-us/windows/wsl/compare-versions) which may be relevant during development so upgrading may be of interest. To do so the following needs to be performed, note that these steps will also enable WSL 2 if WSL 1 has not already been installed.

Open PowerSHell with administrator permissions and enter the following:

```PowerShell
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
```

Once complete, restart the computer, after which the version needs to be set in PowerShell:

```PowerShell
wsl --set-default-version 2
```

If you have already installed a distribution, updating is recommended. Otherwise, you can now install a WSL compliant distribution.

# Building

## Local WSL Builds

Before building the first time it is necessary to create the build directory:

```bash
mkdir build
cd build
```

After which the following command can be run to build the simulation under WSL:

```bash
cmake -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_WSL:BOOL=true ..
make -j 8
```

Generally it is recommended to create a `build.sh` script that runs the build commands.

## Building on ICDS-ACI
To build the simulation to run on the ICDS-ACI the first time it is necessary to perform a number of configuration steps. After logging on to the interactive environment (`aci-b.aci.ics.psu.edu`) cloning this repository run `config.sh` which will prepare the build environment. As part of the process a build script will be created at `build/build.sh` that will ensure the environment is set correctly when run. 

# Running
## Local Runs
The first step in performing a basic model check is load the genome data in the database, this must also be done whenever a new database is corrected:

```base
cd build/bin
./MaSim -i ../../misc/input.yml -l
```

Once the genome data has been loaded the basic input file can be run as follows:

```bash
cd build/bin
./MaSim -i ../../misc/input.yml
```

Note that while care was taken in places to ensure the code is performant, the amount of RAM needed during execution can be quite high (ex., 32 GB or more). When the model is run in Linux environments where the necessary memory is not available, you may find that the program is killed without notice due to being [out of memory](https://linux-mm.org/OOM_Killer).

## Cluster Runs
The [Roar Supercomputer Users' Guide](https://www.icds.psu.edu/computing-services/roar-user-guide/) providers a good overview for running single replicates on the cluster; however, when running batches it is recommended to script out the process. When replicates need to be run with a variety of settings (e.g., sensitivity analysis) some scripts present in [PSU-CIDD-MaSim-Support](https://github.com/bonilab/PSU-CIDD-MaSim-Support) under the `bash` directory can be used to parse a CSV formatted list of replicates to be run. In addition to the [`calibrationLib.sh`](https://github.com/bonilab/PSU-CIDD-MaSim-Support/tree/master/bash) file the support repository, the following files need to be created for this:

1. A runner script which will be queued on the cluster as a job, typically named `run.sh` or similar in project repositories:
```bash
#!/bin/bash
source ./calibrationLib.sh
runReplicates 'replicates.csv' '[USERNAME]'
```

2. The Portable Batch System (PBS) file that defines the job for `run.sh`:
```bash
#!/bin/bash

#PBS -A [ALLOCATION]
#PBS -l nodes=1:ppn=1:rhel7:stmem
#PBS -l pmem=4gb
#PBS -l walltime=120:00:00

#PBS -m ea

cd $PBS_O_WORKDIR
./run.sh
```

When defining the PBS file note the low memory usage (`pmem`) and high `walltime`. Since the job will only be responsible for running this script, only a limited amount of resources is needed. However, the total batch of jobs may run for quite some time, so the wall clock time is likely to be quite high. 

3. The CSV file that defines the replicates to be run, where the first column is the PBS file for the replicate and the second column is the count:
```CSV
bfa-slow-no-asaq.job,1
bfa-fast-no-asaq.job,1
bfa-rapid.job,1
```

While the `runReplicates` command executes, the number of jobs per user account is limited to the `LIMIT` defined in `calibrationLib.sh` (99 by default). When the limit is reached the script will sleep and periodically awaken to check to see if more jobs can be queued.

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

Note that `valgrind` adds **considerable** overhead to the execution of the simulation, so it is highly recommended that profiling be limited to small simulations. 

### Valgrind under WSL 2 with CLion integration

When using CLion as an IDE, integration with Valgrind is possible. Presuming that WSL 2 has been enabled and `valgrind` has been installed, CLion simply needs to be informed of the path under WLS 2 via **File > Settings > Build, Execution, Deployment > Valgrind** and the path will typically be `/usr/bin/valgrind`. 

# Troubleshooting

## WSL on the PSU VPN
One minor problem that may occur while on the PSU VPN is that psu.edu domains are not resolved correctly. This is usually due to the `/etc/resolv.conf` file not being updated correctly by WSL. To manually update the file, launch PowerShell and run,

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
