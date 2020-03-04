#!/usr/bin/env bash
rm -rf ./build_output
mkdir -p build_output

cd ./build_output
cmake -DCMAKE_SYSTEM_PROCESSOR="Arm" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../DockerToolchain.cmake ../
#cmake --build .
make -j 4
cd ..

