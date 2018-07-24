#!/usr/bin/env bash
git pull
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/storage/home/uut47/work/dev_env/vcpkg/scripts/buildsystems/vcpkg.cmake ..
make -j 8