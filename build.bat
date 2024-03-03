@echo off

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=RayText -G "MinGW Makefiles"
cmake --build build --config Debug --parallel 8
copy .\build\RayText.exe .\RayText.exe

if "%1" == "run" (
    echo -----------Run RayText-----------
    RayText.exe
)