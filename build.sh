#!/bin/bash
set -e


cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=RayText > /dev/null
cmake --build build --config Debug --parallel 8
cp ./build/RayText ../RayText

if [ "$1" = "run" ]; then
    echo "-----------Run RayText-----------"
    ./RayText
fi