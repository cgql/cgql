cmake_minimum_required(VERSION 3.10)
project(cgql)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include_directories(utils)
include_directories(schema)
file(GLOB SOURCES
  schema/*.cpp
  utils/pch.h
  schema/*.h
  main.cpp
)
add_executable(cgql ${SOURCES})