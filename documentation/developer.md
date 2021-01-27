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
git checkout 7.0.0
./configure --disable-documentation
make
sudo make install
```

This may need to be done every time you connect to the VPN, so scripting the update may be in order.

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

Generally it is recomended to create a `build.sh` script that runs the build commands.

## Building on ICDS-ACI
To build the simulation to run on the ICDS-ACI the first time it is necessary to perform a number of configuration steps. After logging on to the interactive environment (`aci-b.aci.ics.psu.edu`) cloning this repository run `config.sh` which will prepare the build environment. As part of the process a build script will be created at `build/build.sh` that will ensure the environment is set correctly when run. 

# Running
The first step in performing a basic model check is load the genome data in the database, this must also be done whenever a new database is corrected:

```base
cd /build/bin
./MaSim -i ../../misc/input.yml -l
```

Once the genome data has been loaded the basic input file can be run as follows:

```bash
cd /build/bin
./MaSim -i ../../misc/input.yml
```

Note that while care was taken in places to ensure the code is performant, the amount of RAM needed during execution can be quite high (ex., 32GB or more). When the model is run in Linux environments where the necessary memory is not available, you may find that the program is killed without notice due to being [out of memory](https://linux-mm.org/OOM_Killer).

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
