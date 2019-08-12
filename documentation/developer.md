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
