#!/usr/bin/env bash

function build() {
  cd build
  cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make

  chmod +x tests/cgqlTests
  ./tests/cgqlTests
}

time build
