#!/bin/bash
set -e


cmake -S .. -B build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=main > /dev/null
cd ./build
make -j 8
cp main ../main
cd ..
rm -r ./build

if [ "$1" = "run" ]; then
    echo "-----------Run RayText-----------"
    ./RayText
fi