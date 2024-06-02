#!/bin/bash

DIRECTORY="Build"

if ! command -v cmake &> /dev/null
then
    echo "cmake could not be found -- Please install cmake"
    exit 1
fi

if [ ! -d "$DIRECTORY" ]; then
  mkdir -p $DIRECTORY
fi

cd $DIRECTORY

# check if ninja is available
if command -v ninja &> /dev/null
then
    cmake -DCMAKE_CXX_STANDARD=23 -G Ninja ..
    ninja
    exit 0
fi

cmake -DCMAKE_CXX_STANDARD=23 ..
make