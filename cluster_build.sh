#!/usr/bin/env bash
git pull
mkdir -p build
cd build
module load gcc/5.3.1
module load boost/1.63.0
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 8