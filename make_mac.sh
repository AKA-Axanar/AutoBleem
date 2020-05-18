#!/usr/bin/env bash
rm -rf ./build_arm
mkdir -p build_arm

cd ./build_arm
cmake -DCMAKE_SYSTEM_PROCESSOR="Arm" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../MacToolchain.cmake ../
#cmake --build .
make -j 4
cd ..

