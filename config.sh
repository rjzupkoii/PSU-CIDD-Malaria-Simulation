#!/usr/bin/env bash

# Start by checking to see if we are running with admin permissions
if [ `id -u` -ne 0 ]; then
  echo 'Please run via sudo'
  exit
fi

# Make sure the apt is up to date before installing the tool chain dependencies
apt -q update
apt install build-essential cmake libgsl-dev libyaml-cpp-dev libfmt-dev

# Check to see if we need to configure apt for PostgreSQL
if [ `apt search postgresql-11 2>/dev/null | wc -l` -eq 2 ]; then
  sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'
  wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
fi

# Install PostgreSQL and libraries
apt install postgresql-10 libpq-dev

# Check if libpqxx exists before continuing
if [ ! -d "../libpqxx" ]; then
  git clone https://github.com/jtv/libpqxx.git
  cd libpqxx
  git checkout 7.8.1
  ./configure --disable-documentation
  make
  sudo make install
fi

# Check if the build directory exists before continuing
if [ ! -d "build" ]; then
  mkdir build
  echo "cmake -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_WSL:BOOL=true .." >> build/build.sh
  echo "make -j `nproc --all`" >> build/build.sh
fi
