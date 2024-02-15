#!/bin/bash
set -e


cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug > /dev/null
cd ./build
make -j 8
cp main ../main
cd ..

if [ "$1" = "run" ]; then
    echo "-----------Run RayText-----------"
    ./main
fi