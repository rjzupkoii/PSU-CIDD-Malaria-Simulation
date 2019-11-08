#!/usr/bin/env bash

function check_version() { test "$(printf '%s\n' "$@" | sort -V | head -n 1)" != "$1"; }

# Load gcc
module load gcc/7.3.1

# Create the directory to work in
source=$(pwd)
mkdir -p ~/work/build_env
cd ~/work/build_env

# If the version of git is to old, load a newer one
required=2.24.0
current=$(git --version | sed "s/git version //g")
if check_version $required $current; then
  echo "Upgrading git..."
  git clone git://git.kernel.org/pub/scm/git/git.git
  cd git
  make configure
  ./configure --prefix=/home
  make all
  export PATH="`pwd`:$PATH"
  cd ..
fi

# If the version of cmake is too old, load a newer one
required="3.15"
current=$(cmake --version | sed "s/cmake version //g")
if check_version $required $current; then
  echo "Upgrading cmake..."
  wget https://cmake.org/files/v3.15/cmake-3.15.0-Linux-x86_64.tar.gz
  tar xf cmake-3.15.0-Linux-x86_64.tar.gz
  rm cmake-3.15.0-Linux-x86_64.tar.gz
  export PATH="`pwd`/cmake-3.15.0-Linux-x86_64/bin/:$PATH"
fi

# If PostgreSQL isn't present, download and install it to the user directory
if [ ! -d "~/work/build_env/postgresql" ]; then
  # Get the source files
  wget https://github.com/postgres/postgres/archive/master.zip
  unzip master.zip
  rm master.zip

  # Build the source files
  cd postgres-master
  ./configure --prefix=$HOME/work/build_env/postgres
  make -j 8
  make install
  cd ..
  rm -rf postgres-master

  # Export the relevent variables  
  export PATH="$HOME/work/build_env/postgres/bin:$PATH"
  export PKG_CONFIG_PATH="$HOME/work/build_env/postgres/lib/pkgconfig:$PKG_CONFIG_PATH"
  export LIBRARY_PATH="$HOME/work/build_env/postgres/lib:$LIBRARY_PATH"

  # Prepare the libpqxx library
  git clone https://github.com/jtv/libpqxx.git
  cd libpqxx
  ./configure --disable-documentation --prefix=$HOME/work/build_env/lib
  make -j 8
  make install
fi

# If vcpkg doesn't already exist as a directory, load it
if [ ! -d "~/work/build_env/vcpkg" ]; then
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  cd ..
fi

# Load the relevent packages
cd vcpkg
./vcpkg install yaml-cpp fmt date args

# Return to the source directory
cd $source

# Create the build script
if [ ! -d "build" ]; then
  mkdir -p build
  cd build
  toolchain="$HOME/work/build_env/vcpkg/scripts/buildsystems/vcpkg.cmake"
  echo "module load gcc/7.3.1" > build.sh
  echo "export PATH=$PATH" >> build.sh
  echo "cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$toolchain .." >> build.sh
  echo "make -j 8" >> build.sh
  chmod +x build.sh
fi
