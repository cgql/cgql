#!/usr/bin/env bash

BUILD_TYPE=Release

if [ ! -d build ]; then
  mkdir build
fi

function build() {
  cmake -B build/ -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  cd build && make -j4 && cd ..
  chmod +x build/tests/cgqlTests
  time build/tests/cgqlTests
}

build
