#!/bin/bash

BUILD_DIR=".build-tests"
mkdir -p "$BUILD_DIR"

echo "Building in $BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_CXX_STANDARD=23 \
    -DBUILD_TESTS=ON \
    -G Ninja

cmake --build "$BUILD_DIR"

ctest --test-dir "$BUILD_DIR" --output-on-failure
