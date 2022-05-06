#!/usr/bin/env bash

BUILD_TYPE=Debug

if [ ! -d build ]; then
  mkdir build
fi

if [[ $1 != "" ]] && [[ $1 = "-c" ]]; then
  cmake -B build/ \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++
fi

function build() {
  cd build && make -j3 && cd ..
  chmod +x build/tests/cgqlTests
  time build/tests/cgqlTests
}

build
