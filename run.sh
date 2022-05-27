#!/usr/bin/env bash

BUILD_TYPE=Release

if [ ! -d build ]; then
  mkdir build
fi

if [[ $1 != "" ]] && [[ $1 = "-c" ]]; then
  cmake -B build/ \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi

function build() {
  cd build && make -j3 && cd ..
  chmod +x build/tests/cgqlTests

  du -h build/tests/cgqlTests build/libcgqlSource.a

  time build/tests/cgqlTests
}

build
